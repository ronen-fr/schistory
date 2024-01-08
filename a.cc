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
#include <boost/statechart/simple_state.hpp>
#include <boost/statechart/shallow_history.hpp>
#include <boost/statechart/state.hpp>
#include <boost/statechart/state_machine.hpp>
#include <boost/statechart/transition.hpp>

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

struct X : sc::simple_state< X, A > {BODY(X)};
struct Y : sc::simple_state< Y, A > {BODY(Y)};
struct Idle;
struct RActive : sc::simple_state< RActive, A, Idle > {BODY(RActive);};
struct Idle : sc::simple_state< Idle, RActive, IUn> {
  BODY(Idle);
        typedef mpl::list<
        sc::transition< EvToIUn, IUn >,
        sc::transition< EvToIMid, IMid >,
        sc::transition< EvToIReg, IReg >
        , sc::custom_reaction< EvToOP >
        > reactions;
  sc::result react( const EvToOP & ) { return transit< OP >(); }
};
struct IUn : sc::simple_state< IUn, Idle > {
  BODY(IUn);
        typedef mpl::list<
                sc::transition< EvToIMid, IMid >,
                sc::transition< EvToIReg, IReg >
                > reactions;
};
struct IMid : sc::simple_state< IMid, Idle > {BODY(IMid);};
struct IReg : sc::simple_state< IReg, Idle > {BODY(IReg);};

struct ActPre;
struct ActPost;
struct OP : sc::simple_state< OP, RActive, ActPre > {BODY(OP);};
struct ActPre : sc::simple_state< ActPre, OP > {
        BODY(ActPre);
        typedef mpl::list<
        //     sc::transition< EvToIUn, IUn >,
        //     sc::transition< EvToIMid, IMid >,
        //     sc::transition< EvToIReg, IReg >,
            sc::transition< EvToPost, ActPost >
        > reactions;
};
struct ActPost : sc::simple_state< ActPost, OP > {BODY(ActPost);};

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
