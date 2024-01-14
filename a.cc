//////////////////////////////////////////////////////////////////////////////
// Copyright 2005-2006 Andreas Huber Doenni
// Distributed under the Boost Software License, Version 1.0. (See accompany-
// ing file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////



#include <boost/statechart/state_machine.hpp>
#include <boost/statechart/simple_state.hpp>
#include <boost/statechart/event.hpp>
#include <boost/statechart/transition.hpp>
#include <boost/statechart/shallow_history.hpp>
#include <boost/statechart/deep_history.hpp>
#include <boost/statechart/custom_reaction.hpp>
#include <boost/statechart/deferral.hpp>
#include <boost/statechart/event.hpp>
#include <boost/statechart/event_base.hpp>
#include <boost/statechart/in_state_reaction.hpp>
#include <boost/statechart/state.hpp>

#include <boost/mpl/list.hpp>
#include <boost/shared_ptr.hpp>

#include <format>
#include <iostream>

//#include <boost/test/test_tools.hpp>

#include <stdexcept>
#include <cassert>


namespace sc = boost::statechart;
namespace mpl = boost::mpl;



struct EvToB : sc::event< EvToB > {};

struct EvToD : sc::event< EvToD > {};
struct EvToDShallow : sc::event< EvToDShallow > {};
struct EvToDDeep : sc::event< EvToDDeep > {};
struct EvToDShallowLocal : sc::event< EvToDShallowLocal > {};
struct EvToDDeepLocal : sc::event< EvToDDeepLocal > {};

struct EvToF : sc::event< EvToF > {};
struct EvToFShallow : sc::event< EvToFShallow > {};
struct EvToFDeep : sc::event< EvToFDeep > {};

struct EvToH : sc::event< EvToH > {};
struct EvToI : sc::event< EvToI > {};

struct EvToM : sc::event< EvToM > {};
struct EvToQ : sc::event< EvToQ > {};

struct EvWhatever : sc::event< EvWhatever > {};


// R events
struct EvToOP: sc::event< EvToOP > {};
struct EvToIUn: sc::event< EvToIUn > {};
struct EvToIMid: sc::event< EvToIMid > {};
struct EvToIReg: sc::event< EvToIReg > {};
struct EvToPost: sc::event< EvToPost > {};
struct EvToUN_sh: sc::event< EvToUN_sh > {};
struct EvToUN_dp: sc::event< EvToUN_dp > {};
struct EvToIdle_sh: sc::event< EvToIdle_sh > {};
struct EvToIdle_dp: sc::event< EvToIdle_dp > {};
struct EvToIdle: sc::event< EvToIdle > {};

struct EvToX: sc::event< EvToX > {};
struct EvToY: sc::event< EvToY > {};

struct EvInterval: sc::event< EvInterval > {};
struct EvToActiMidPlus: sc::event< EvToActiMidPlus > {};


struct A;
struct HistoryTest : sc::state_machine< HistoryTest, A >
{
  void unconsumed_event( const sc::event_base & )
  {
    throw std::runtime_error( "Event was not consumed!" );
  }
};

struct OP;
struct IUn;
struct IMid;
struct IReg;
struct B;
struct D;
struct F;
struct H;
struct I;
struct M;
struct Q;
struct X;
struct ActPre;
struct ActMid;
struct ActPost;
struct Idle;

struct A : sc::simple_state< A, HistoryTest, X >
{
  typedef mpl::list<
    sc::transition< EvToB, B >,
    sc::transition< EvToD, D >,
    sc::transition< EvToDShallow, sc::shallow_history< D > >,
    sc::transition< EvToDDeep, sc::deep_history< D > >,
    sc::transition< EvToF, F >,
    sc::transition< EvToFShallow, sc::shallow_history< F > >,
    sc::transition< EvToFDeep, sc::deep_history< F > >,
    sc::transition< EvToH, H >,
    sc::transition< EvToI, I >,
    sc::transition< EvToM, M >,
    sc::transition< EvToQ, Q >


  //, sc::custom_reaction< EvToOP >
  , sc::transition< EvToIUn, IUn >
  , sc::transition< EvToIMid, IMid >
  //, sc::transition< EvToIReg, IReg >
  > reactions;

  sc::result react( const EvToOP & ) { return transit< OP >(); }
};

#define BODY(S) \
  S() {std::cout << #S << "\n";} \
  ~S() {std::cout << "~" << #S << "\n";}

struct X : sc::simple_state< X, A > {
        BODY(X)
        typedef mpl::list<
        sc::transition< EvToIUn, IUn >
        , sc::transition< EvToOP, OP >
        , sc::transition< EvToIMid, IMid >
        , sc::transition< EvToIReg, IReg >
        , sc::transition< EvToIdle, Idle >
        > reactions;
};

/// Y is used to test 'sc::result' forwarding
struct Y : sc::simple_state< Y, A > {
  sc::result aux(int i) {
    std::cout << "aux(" << i << ")\n";
    switch (i) {
      case 0: return transit< X >();
      case 1: return transit< IUn >();
      case 2: return transit< IMid >();
      case 3: return transit< IReg >();
      case 4: return transit< Idle >();
      default: return discard_event();
    }
    return discard_event();
  }

//  This won't compile:
//   sc::result cpaux(int i) {
//     std::cout << "cpaux(" << i << ")\n";
//     sc::result r{discard_event()};
//     switch (i) {
//       case 0: r = std::move(transit< X >()); break;
//       case 1: r =  transit< IUn >(); break;
//       case 2: r =  transit< IMid >(); break;
//       case 3: r =  transit< IReg >(); break;
//       case 4: r =  transit< Idle >(); break;
//       default: r =  discard_event(); break;
//     }
//     return r;
//   }


BODY(Y)

        typedef mpl::list<
        sc::custom_reaction< EvToIUn >
        , sc::custom_reaction< EvToIMid >
        , sc::custom_reaction< EvToIReg >
        , sc::custom_reaction< EvToIdle >
        > reactions;

        sc::result react( const EvToIUn & ) { return aux(1); }
        sc::result react( const EvToIMid & ) { return aux(2); }
        sc::result react( const EvToIReg & ) { return aux(3); }
        sc::result react( const EvToIdle & ) { return aux(4); }
};


// the 'has shallow' here is required
struct RActive : sc::simple_state< RActive, A, mpl::list<sc::shallow_history<Idle>>, sc::has_shallow_history>
{
  BODY(RActive);
        typedef mpl::list<
        sc::transition< EvInterval, A >
        > reactions;
};


struct Idle : sc::simple_state< Idle, RActive, IUn, sc::has_shallow_history> {
  BODY(Idle);
        typedef mpl::list<
        sc::transition< EvToIUn, IUn >,
        sc::transition< EvToIMid, IMid >,
        sc::transition< EvToIReg, IReg >
        , sc::transition< EvToX, X >
        , sc::custom_reaction< EvToOP >
        > reactions;
  sc::result react( const EvToOP & ) { return transit< OP >(); }
};
struct IUn : sc::simple_state< IUn, Idle > {
  BODY(IUn);
        typedef mpl::list<
                sc::transition< EvToIMid, IMid >,
                sc::transition< EvToIReg, IReg >,
                sc::transition< EvToY, Y >
                > reactions;
};
struct IMid : sc::simple_state< IMid, Idle > {
        BODY(IMid);
        typedef mpl::list<
                sc::transition< EvToIUn, IUn >
                , sc::transition< EvToIReg, IReg >
                > reactions;};
struct IReg : sc::simple_state< IReg, Idle > {
  BODY(IReg);
        typedef mpl::list<
                sc::transition< EvToIUn, IUn >
                , sc::transition< EvToIMid, IMid >
                , sc::transition< EvToPost, ActPost >
                > reactions;
};

//works, too: struct OP : sc::simple_state< OP, RActive, mpl::list<sc::shallow_history<ActPre>> , sc::has_full_history> {
struct OP : sc::simple_state< OP, RActive, ActPre > {
        BODY(OP);
        typedef mpl::list<
        sc::transition< EvToIUn, IUn >
        , sc::transition< EvToIMid, IMid >
        , sc::transition< EvToIReg, IReg >
        , sc::transition< EvToUN_sh, sc::shallow_history<IUn> >
        , sc::transition< EvToX, X >
        > reactions;
};


struct ActPre : sc::simple_state< ActPre, OP > {
        BODY(ActPre);
        typedef mpl::list<
        //     sc::transition< EvToIUn, IUn >,
        //     sc::transition< EvToIMid, IMid >,
        //     sc::transition< EvToIReg, IReg >,
            sc::transition< EvToPost, ActPost >
        , sc::custom_reaction< EvToActiMidPlus >
        , sc::transition< EvToUN_sh, sc::shallow_history<IUn> >
        > reactions;

        sc::result react( const EvToActiMidPlus & ) {
                std::cout << "ActPre::react(EvToActiMidPlus)\n";
                post_event(EvToPost());
                return transit< ActMid >();
        }
};

struct ActMid : sc::simple_state< ActMid, OP > {
        BODY(ActMid);
        typedef mpl::list<
        //     sc::transition< EvToIUn, IUn >,
        //     sc::transition< EvToIMid, IMid >,
        //     sc::transition< EvToIReg, IReg >,
            sc::transition< EvToPost, ActPost >
        , sc::transition< EvToUN_sh, sc::shallow_history<IUn> >
        > reactions;
};

struct ActPost : sc::simple_state< ActPost, OP> {
        BODY(ActPost);
        typedef mpl::list<
        sc::transition< EvToIUn, IUn >
        , sc::transition< EvToIMid, IMid >
        , sc::transition< EvToIReg, IReg >
        , sc::transition< EvToUN_sh, sc::shallow_history<IUn> >
        , sc::transition< EvToIdle_sh, sc::shallow_history<Idle> >
        //, sc::transition< EvToIdle_dp, sc::deep_history<Idle> >
        , sc::transition< EvToIdle, Idle >
        > reactions;
};

void basic_h(boost::shared_ptr< HistoryTest > pM)
{
  std::cout << "\n\n=================\n" << __func__ << "\n";
  pM->initiate();
  std::cout << "--- -----------------\n" << __func__ << "\n";
  pM->process_event( EvToIUn() );
  assert(pM->state_downcast< const IUn * >());
  pM->process_event( EvToIReg() );
  assert(pM->state_downcast< const IReg * >());

  // now - move to OP
  std::cout << "\n--- going for OP\n";

  pM->process_event( EvToOP() );
  assert(pM->state_downcast< const ActPre * >());

  // temp test:
  if (false) {
  std::cout << "\n--- un sh\n";
  pM->process_event( EvToUN_sh() );
  std::cout << (pM->state_downcast< const Idle * >() ? "in Idle" : "not Idle") << "\n";
  std::cout << (pM->state_downcast< const IUn * >() ? "in IUn" : "not IUn") << "\n";
  std::cout << (pM->state_downcast< const IReg * >() ? "in IReg" : "not IReg") << "\n";
  std::cout << (pM->state_downcast< const ActPost * >() ? "in ActPost" : "not ActPost") << "\n";
  }

  // continue to post
  pM->process_event( EvToPost() );
  assert(pM->state_downcast< const ActPost * >());

  // and back with history
  std::cout << "\n--- back to some form of Idle\n";
  pM->process_event( EvToUN_sh() );
  //pM->process_event( EvToIdle_dp() );
  std::cout << (pM->state_downcast< const Idle * >() ? "in Idle" : "not Idle") << "\n";
  std::cout << (pM->state_downcast< const IUn * >() ? "in IUn" : "not IUn") << "\n";
  std::cout << (pM->state_downcast< const IReg * >() ? "in IReg" : "not IReg") << "\n";
  std::cout << (pM->state_downcast< const ActPost * >() ? "in ActPost" : "not ActPost") << "\n";

  //pM->process_event( EvToIdle() );
  std::cout << (pM->state_downcast< const Idle * >() ? "in Idle" : "not Idle") << "\n";
  std::cout << (pM->state_downcast< const IUn * >() ? "in IUn" : "not IUn") << "\n";
  std::cout << (pM->state_downcast< const IReg * >() ? "in IReg" : "not IReg") << "\n";
  std::cout << (pM->state_downcast< const ActPost * >() ? "in ActPost" : "not ActPost") << "\n";

  std::cout << "\n--- done\n";
}


void basic_this_works(boost::shared_ptr< HistoryTest > pM)
{
  std::cout << "\n\n=================\n" << __func__ << "\n";
  pM->initiate();
  std::cout << "--- -----------------\n" << __func__ << "\n";
  pM->process_event( EvToIUn() );
  assert(pM->state_downcast< const IUn * >());
  pM->process_event( EvToIReg() );
  assert(pM->state_downcast< const IReg * >());

  // now - move to OP
  std::cout << "\n--- going for OP\n";

  pM->process_event( EvToOP() );
  assert(pM->state_downcast< const ActPre * >());

  std::cout << "\n--- un sh\n";
  pM->process_event( EvToUN_sh() );
  std::cout << (pM->state_downcast< const Idle * >() ? "in Idle" : "not Idle") << "\n";
  std::cout << (pM->state_downcast< const IUn * >() ? "in IUn" : "not IUn") << "\n";
  std::cout << (pM->state_downcast< const IReg * >() ? "in IReg" : "not IReg") << "\n";
  std::cout << (pM->state_downcast< const ActPost * >() ? "in ActPost" : "not ActPost") << "\n";
  assert(pM->state_downcast< const IReg * >());
}


void forget1(boost::shared_ptr< HistoryTest > pM, bool do_forget)
{
  std::cout << "\n\n=================  " << __func__ << "\n";
  pM->initiate();
  std::cout << "--- -----------------   " << __func__ << "\n";
  pM->process_event( EvToIUn() );
  assert(pM->state_downcast< const IUn * >());
  pM->process_event( EvToIReg() );
  assert(pM->state_downcast< const IReg * >());

  // to X and back. Not really needed for now
        std::cout << "\n--- going for X\n";
        pM->process_event( EvToX() );
        assert(pM->state_downcast< const X * >());
        std::cout << "\n--- going for Idle\n";
        pM->process_event( EvToIdle() );
        assert(pM->state_downcast< const Idle * >());
        assert(pM->state_downcast< const IUn * >());

  pM->process_event( EvToIReg() );
  assert(pM->state_downcast< const IReg * >());

 // now - move to OP
  std::cout << "\n--- going for OP\n";

  pM->process_event( EvToOP() );
  assert(pM->state_downcast< const ActPre * >());
  if (do_forget)
    pM->clear_shallow_history< Idle, 0 >();
  //pM->clear_shallow_history< IUn, 0 >();

  std::cout << "\n--- un sh\n";
  pM->process_event( EvToUN_sh() );
  std::cout << (pM->state_downcast< const Idle * >() ? "in Idle" : "not Idle") << "\n";
  std::cout << (pM->state_downcast< const IUn * >() ? "in IUn" : "not IUn") << "\n";
  std::cout << (pM->state_downcast< const IReg * >() ? "in IReg" : "not IReg") << "\n";
  std::cout << (pM->state_downcast< const ActPost * >() ? "in ActPost" : "not ActPost") << "\n";
  if (do_forget) {
    assert(pM->state_downcast< const IUn * >());
  } else {
    assert(pM->state_downcast< const IReg * >());
  }

  std::cout << "--- done " << __func__ << "\n";
}

/*
  We really only need the history if going deeper from Idle, i.e.:
  - if we were in IReg or IUn, then left IDLE and came back, we should be in IReg or IUn.
  But if we leave IDLE parent (RACTIVE) and come back, teh history must be erased!
  This was not the case on 12.1.24 code.
*/
void correct_level(boost::shared_ptr<HistoryTest> pM) {
  std::cout << "\n\n=================  " << __func__ << "\n";
  pM->initiate();
  std::cout << "--- -----------------  " << __func__ << "\n";

  // First: ->OP/ActPre -> X -> Idle/Un
  pM->process_event(EvToOP());
  assert(pM->state_downcast<const ActPre*>());
  pM->process_event(EvToX());
  assert(pM->state_downcast<const X*>());
  pM->process_event(EvToIdle());
  assert(pM->state_downcast<const Idle*>());
  assert(pM->state_downcast<const IUn*>());


  // 2'nd: ->Idle/Ireg  ->OP/ActPre -> X -> Idle/Un
  std::cout << "\n\n2'nd: ->Idle/Ireg  ->OP/ActPre -> X -> Idle/Un\n";
  pM->process_event(EvInterval());
  assert(pM->state_downcast<const X*>());
  pM->process_event(EvToIdle());
  assert(pM->state_downcast<const Idle*>());
  assert(pM->state_downcast<const IUn*>());
  pM->process_event( EvToIReg() );
  assert(pM->state_downcast< const IReg * >());


  pM->process_event(EvToOP());
  assert(pM->state_downcast<const ActPre*>());
  pM->process_event(EvToActiMidPlus());
  assert(pM->state_downcast<const ActPost*>());

  pM->process_event(EvInterval());
  assert(pM->state_downcast<const X*>());
  pM->process_event(EvToIdle());
  assert(pM->state_downcast<const Idle*>());
  assert(pM->state_downcast<const IUn*>());
}

void basic_this_works2(boost::shared_ptr< HistoryTest > pM)
{
  std::cout << "\n\n=================\n" << __func__ << "\n";
  pM->initiate();
  std::cout << "--- -----------------\n" << __func__ << "\n";
  pM->process_event( EvToIUn() );
  assert(pM->state_downcast< const IUn * >());
  pM->process_event( EvToIReg() );
  assert(pM->state_downcast< const IReg * >());

  // now - move to OP
  std::cout << "\n--- going for OP\n";

  pM->process_event( EvToOP() );
  assert(pM->state_downcast< const ActPre * >());
  // continue to post
  pM->process_event( EvToPost() );
  assert(pM->state_downcast< const ActPost * >());

  std::cout << "\n--- un sh\n";
  #if 0
  // this one (EvToUN_sh) - works
  pM->process_event( EvToUN_sh() );
  #else
  // this one too. Same effect.
  pM->process_event( EvToIdle_sh() );
  #endif
  std::cout << (pM->state_downcast< const Idle * >() ? "in Idle" : "not Idle") << "\n";
  std::cout << (pM->state_downcast< const IUn * >() ? "in IUn" : "not IUn") << "\n";
  std::cout << (pM->state_downcast< const IReg * >() ? "in IReg" : "not IReg") << "\n";
  std::cout << (pM->state_downcast< const ActPost * >() ? "in ActPost" : "not ActPost") << "\n";
  assert(pM->state_downcast< const IUn * >());
}

void test_forwarding(boost::shared_ptr< HistoryTest > pM)
{
  std::cout << "\n\n=================\n" << __func__ << "\n";
  pM->initiate();
  std::cout << "--- -----------------\n" << __func__ << "\n";
  pM->process_event( EvToIUn() );
  assert(pM->state_downcast< const IUn * >());
  pM->process_event( EvToY() );
  assert(pM->state_downcast< const Y * >());
  pM->process_event( EvToIReg() );
  assert(pM->state_downcast< const IReg * >());
}

int main( int, char* [] )
{
  boost::shared_ptr< HistoryTest > pM =
    boost::shared_ptr< HistoryTest >( new HistoryTest() );

  pM->initiate();
  pM->process_event( EvToIUn() );
  assert(pM->state_downcast< const IUn * >());
  pM->process_event( EvToIReg() );
  assert(pM->state_downcast< const IReg * >());

  // now - move to OP
  std::cout << "\n--- going for OP\n";

  pM->process_event( EvToOP() );
  assert(pM->state_downcast< const ActPre * >());

  // continue to post
  pM->process_event( EvToPost() );
  assert(pM->state_downcast< const ActPost * >());

  // and back to idle
  pM->process_event( EvToIMid() );
  assert(pM->state_downcast< const IMid * >());

  // 13.1.2024:
  correct_level(pM);

  forget1(pM, true);
  forget1(pM, false);
//   test_forwarding(pM);
//   basic_this_works(pM);
//   //basic_this_works2(pM);
//   basic_h(pM);

}


  struct J;
  struct N;
  struct B : sc::simple_state<
    B, A, mpl::list< sc::shallow_history< J >, sc::deep_history< N > >,
    sc::has_full_history > { 
      B() {std::cout << "B\n";}
};

    struct J : sc::simple_state< J, B::orthogonal< 0 > > {};
    struct L;
    struct K : sc::simple_state< K, B::orthogonal< 0 >, L > {};

      struct L : sc::simple_state< L, K > {};
      struct M : sc::simple_state< M, K > {};

    struct N : sc::simple_state< N, B::orthogonal< 1 > > {};
    struct P;
    struct O : sc::simple_state< O, B::orthogonal< 1 >, P > {};

      struct P : sc::simple_state< P, O > {};
      struct Q : sc::simple_state< Q, O > {};

  struct C : sc::simple_state< C, A, D, sc::has_full_history > {};

    struct D : sc::simple_state< D, C > {};
    struct E : sc::simple_state< E, C, F, sc::has_full_history > {};

      struct F : sc::simple_state< F, E > {};
      struct G : sc::simple_state< G, E, H >
      {
        typedef mpl::list<
          sc::transition< EvToDShallowLocal, sc::shallow_history< D > >,
          sc::transition< EvToDDeepLocal, sc::deep_history< D > >
        > reactions;
      };

        struct H : sc::simple_state< H, G > {};
        struct I : sc::simple_state< I, G > {};


int main1( int, char* [] )
{
  boost::shared_ptr< HistoryTest > pM =
    boost::shared_ptr< HistoryTest >( new HistoryTest() );

  pM->initiate();
  pM->process_event( EvToDShallow() );

#if 0
  // state_downcast sanity check
  BOOST_REQUIRE_THROW( pM->state_downcast< const B & >(), std::bad_cast );
  pM->initiate();
  BOOST_REQUIRE_THROW( pM->state_downcast< const D & >(), std::bad_cast );

  BOOST_REQUIRE_NO_THROW( pM->state_downcast< const B & >() );

  // No history has been saved yet -> default state
  pM->process_event( EvToDShallow() );
  BOOST_REQUIRE_NO_THROW( pM->state_downcast< const D & >() );
  pM->process_event( EvToDShallow() );
  BOOST_REQUIRE_NO_THROW( pM->state_downcast< const D & >() );

  pM->process_event( EvToI() );
  BOOST_REQUIRE_NO_THROW( pM->state_downcast< const I & >() );
  // Direct inner is E when history is saved -> F
  pM->process_event( EvToDShallow() );
  BOOST_REQUIRE_NO_THROW( pM->state_downcast< const F & >() );

  pM->process_event( EvToH() );
  BOOST_REQUIRE_NO_THROW( pM->state_downcast< const H & >() );
  // Direct inner is E when history is saved -> F
  pM->process_event( EvToDShallow() );
  BOOST_REQUIRE_NO_THROW( pM->state_downcast< const F & >() );

  pM->process_event( EvToF() );
  BOOST_REQUIRE_NO_THROW( pM->state_downcast< const F & >() );
  pM->process_event( EvToB() );
  BOOST_REQUIRE_NO_THROW( pM->state_downcast< const B & >() );
  // Direct inner was E when history was saved -> F
  pM->process_event( EvToDShallow() );
  BOOST_REQUIRE_NO_THROW( pM->state_downcast< const F & >() );

  pM->initiate();
  // History was cleared in termination -> default state
  pM->process_event( EvToDShallow() );
  BOOST_REQUIRE_NO_THROW( pM->state_downcast< const D & >() );

  pM->process_event( EvToI() );
  BOOST_REQUIRE_NO_THROW( pM->state_downcast< const I & >() );
  pM->process_event( EvToB() );
  BOOST_REQUIRE_NO_THROW( pM->state_downcast< const B & >() );
  pM->clear_shallow_history< C, 0 >();
  // History was cleared -> default state
  pM->process_event( EvToDShallow() );
  BOOST_REQUIRE_NO_THROW( pM->state_downcast< const D & >() );

  pM = boost::shared_ptr< HistoryTest >( new HistoryTest() );
  pM->initiate();
  BOOST_REQUIRE_NO_THROW( pM->state_downcast< const B & >() );

  // No history has been saved yet -> default state
  pM->process_event( EvToDDeep() );
  BOOST_REQUIRE_NO_THROW( pM->state_downcast< const D & >() );
  pM->process_event( EvToDDeep() );
  BOOST_REQUIRE_NO_THROW( pM->state_downcast< const D & >() );

  pM->process_event( EvToI() );
  BOOST_REQUIRE_NO_THROW( pM->state_downcast< const I & >() );
  pM->process_event( EvToB() );
  BOOST_REQUIRE_NO_THROW( pM->state_downcast< const B & >() );
  pM->process_event( EvToDDeep() );
  BOOST_REQUIRE_NO_THROW( pM->state_downcast< const I & >() );

  pM->process_event( EvToH() );
  BOOST_REQUIRE_NO_THROW( pM->state_downcast< const H & >() );
  pM->process_event( EvToB() );
  BOOST_REQUIRE_NO_THROW( pM->state_downcast< const B & >() );
  pM->process_event( EvToDDeep() );
  BOOST_REQUIRE_NO_THROW( pM->state_downcast< const H & >() );

  pM->process_event( EvToF() );
  BOOST_REQUIRE_NO_THROW( pM->state_downcast< const F & >() );
  pM->process_event( EvToB() );
  BOOST_REQUIRE_NO_THROW( pM->state_downcast< const B & >() );
  pM->process_event( EvToDDeep() );
  BOOST_REQUIRE_NO_THROW( pM->state_downcast< const F & >() );

  pM->initiate();
  // History was cleared in termination -> default state
  pM->process_event( EvToDDeep() );
  BOOST_REQUIRE_NO_THROW( pM->state_downcast< const D & >() );

  pM->process_event( EvToI() );
  BOOST_REQUIRE_NO_THROW( pM->state_downcast< const I & >() );
  pM->process_event( EvToB() );
  BOOST_REQUIRE_NO_THROW( pM->state_downcast< const B & >() );
  pM->clear_deep_history< C, 0 >();
  // History was cleared -> default state
  pM->process_event( EvToDDeep() );
  BOOST_REQUIRE_NO_THROW( pM->state_downcast< const D & >() );


  pM = boost::shared_ptr< HistoryTest >( new HistoryTest() );
  pM->initiate();
  BOOST_REQUIRE_NO_THROW( pM->state_downcast< const B & >() );

  // No history has been saved yet -> default state
  pM->process_event( EvToFShallow() );
  BOOST_REQUIRE_NO_THROW( pM->state_downcast< const F & >() );
  pM->process_event( EvToFShallow() );
  BOOST_REQUIRE_NO_THROW( pM->state_downcast< const F & >() );

  pM->process_event( EvToI() );
  BOOST_REQUIRE_NO_THROW( pM->state_downcast< const I & >() );
  // Direct inner is G when history is saved -> H
  pM->process_event( EvToFShallow() );
  BOOST_REQUIRE_NO_THROW( pM->state_downcast< const H & >() );

  pM->process_event( EvToH() );
  BOOST_REQUIRE_NO_THROW( pM->state_downcast< const H & >() );
  pM->process_event( EvToB() );
  BOOST_REQUIRE_NO_THROW( pM->state_downcast< const B & >() );
  // Direct inner was G when history was saved -> H
  pM->process_event( EvToFShallow() );
  BOOST_REQUIRE_NO_THROW( pM->state_downcast< const H & >() );

  pM->process_event( EvToI() );
  BOOST_REQUIRE_NO_THROW( pM->state_downcast< const I & >() );
  pM->initiate();
  // History was cleared in termination -> default state
  pM->process_event( EvToFShallow() );
  BOOST_REQUIRE_NO_THROW( pM->state_downcast< const F & >() );

  pM->process_event( EvToI() );
  BOOST_REQUIRE_NO_THROW( pM->state_downcast< const I & >() );
  pM->process_event( EvToB() );
  BOOST_REQUIRE_NO_THROW( pM->state_downcast< const B & >() );
  pM->clear_shallow_history< E, 0 >();
  // History was cleared -> default state
  pM->process_event( EvToFShallow() );
  BOOST_REQUIRE_NO_THROW( pM->state_downcast< const F & >() );

  pM = boost::shared_ptr< HistoryTest >( new HistoryTest() );
  pM->initiate();
  BOOST_REQUIRE_NO_THROW( pM->state_downcast< const B & >() );

  // No history has been saved yet -> default state
  pM->process_event( EvToFDeep() );
  BOOST_REQUIRE_NO_THROW( pM->state_downcast< const F & >() );
  pM->process_event( EvToFDeep() );
  BOOST_REQUIRE_NO_THROW( pM->state_downcast< const F & >() );

  pM->process_event( EvToI() );
  BOOST_REQUIRE_NO_THROW( pM->state_downcast< const I & >() );
  pM->process_event( EvToB() );
  BOOST_REQUIRE_NO_THROW( pM->state_downcast< const B & >() );
  pM->process_event( EvToFDeep() );
  BOOST_REQUIRE_NO_THROW( pM->state_downcast< const I & >() );

  pM->process_event( EvToH() );
  BOOST_REQUIRE_NO_THROW( pM->state_downcast< const H & >() );
  pM->process_event( EvToB() );
  BOOST_REQUIRE_NO_THROW( pM->state_downcast< const B & >() );
  pM->process_event( EvToFDeep() );
  BOOST_REQUIRE_NO_THROW( pM->state_downcast< const H & >() );

  pM->process_event( EvToF() );
  BOOST_REQUIRE_NO_THROW( pM->state_downcast< const F & >() );
  pM->process_event( EvToB() );
  BOOST_REQUIRE_NO_THROW( pM->state_downcast< const B & >() );
  pM->process_event( EvToFDeep() );
  BOOST_REQUIRE_NO_THROW( pM->state_downcast< const F & >() );

  pM->initiate();
  // History was cleared in termination -> default state
  pM->process_event( EvToFDeep() );
  BOOST_REQUIRE_NO_THROW( pM->state_downcast< const F & >() );

  pM->process_event( EvToI() );
  BOOST_REQUIRE_NO_THROW( pM->state_downcast< const I & >() );
  pM->process_event( EvToB() );
  BOOST_REQUIRE_NO_THROW( pM->state_downcast< const B & >() );
  pM->clear_deep_history< E, 0 >();
  // History was cleared -> default state
  pM->process_event( EvToFDeep() );
  BOOST_REQUIRE_NO_THROW( pM->state_downcast< const F & >() );

  // Test local history (new with UML 2.0)
  pM->initiate();
  // unconsumed_event sanity check
  BOOST_REQUIRE_THROW( pM->process_event( EvWhatever() ), std::runtime_error );
  pM->process_event( EvToI() );
  BOOST_REQUIRE_NO_THROW( pM->state_downcast< const I & >() );
  pM->process_event( EvToDShallowLocal() );
  BOOST_REQUIRE_NO_THROW( pM->state_downcast< const F & >() );
  pM->process_event( EvToI() );
  pM->process_event( EvToDDeepLocal() );
  BOOST_REQUIRE_NO_THROW( pM->state_downcast< const I & >() );

  // Given that history transitions and history initial states are implemented
  // with the same code we just make a few sanity checks and trust that the
  // rest will work just like we tested above.
  pM->process_event( EvToB() );
  BOOST_REQUIRE_NO_THROW( pM->state_downcast< const J & >() );
  BOOST_REQUIRE_NO_THROW( pM->state_downcast< const N & >() );
  pM->process_event( EvToM() );
  BOOST_REQUIRE_NO_THROW( pM->state_downcast< const M & >() );
  // Direct inner is K when history is saved -> L
  pM->process_event( EvToB() );
  BOOST_REQUIRE_NO_THROW( pM->state_downcast< const L & >() );

  BOOST_REQUIRE_NO_THROW( pM->state_downcast< const N & >() );
  pM->process_event( EvToQ() );
  BOOST_REQUIRE_NO_THROW( pM->state_downcast< const Q & >() );
  pM->process_event( EvToB() );
  BOOST_REQUIRE_NO_THROW( pM->state_downcast< const Q & >() );
#endif
  return 0;
}
