#ifndef PHALANX_EVALAUTOR_TASK_BASE_HPP
#define PHALANX_EVALAUTOR_TASK_BASE_HPP

#ifdef PHX_ENABLE_KOKKOS_AMT

#include "Phalanx_config.hpp"
#include "Phalanx_Evaluator_WithBaseImpl.hpp"
#include "Workset.hpp"

namespace PHX_example {

  // Generic task wrapper for a kokkos functor
  template<typename Space,typename Functor>
  struct TaskWrap {
    
    typedef void value_type;
    typedef Kokkos::Experimental::TaskPolicy<Space> policy_type;
    
    const int work_size;
    const Functor& functor;

    TaskWrap(const int ws,const Functor& f) : work_size(ws),functor(f) {}
    
    void apply(const typename policy_type::member_type & member)
    {
      Kokkos::parallel_for(Kokkos::TeamThreadRange(member,work_size),functor);
    }
  };

  
  template<typename Traits,typename Derived>
  struct TaskBase : public PHX::EvaluatorWithBaseImpl<Traits> {

    virtual Kokkos::Experimental::Future<void,PHX::Device::execution_space>
    createTask(Kokkos::Experimental::TaskPolicy<PHX::Device::execution_space>& policy,
	       const std::size_t& num_adjacencies,
	       typename Traits::EvalData d) override
    {
      return policy.task_create_team(PHX_example::TaskWrap<PHX::Device::execution_space,Derived>(d.num_cells,*dynamic_cast<Derived*>(this)),num_adjacencies);
    }
  };

}

#endif

#endif
