// @HEADER
// ***********************************************************************
//
//           Panzer: A partial differential equation assembly
//       engine for strongly coupled complex multiphysics systems
//                 Copyright (2011) Sandia Corporation
//
// Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
// the U.S. Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact Roger P. Pawlowski (rppawlo@sandia.gov) and
// Eric C. Cyr (eccyr@sandia.gov)
// ***********************************************************************
// @HEADER

#include "Panzer_STK_SetupPartitionedWorksetUtilities.hpp"

#include "Panzer_STK_LocalPartitioningUtilities.hpp"

#include "Panzer_STK_SetupUtilities.hpp"
#include "Panzer_STK_Interface.hpp"
#include "Panzer_Workset_Builder.hpp"

#include "Panzer_Workset.hpp"
#include "Panzer_WorksetNeeds.hpp"
#include "Panzer_WorksetDescriptor.hpp"

#include "Teuchos_Assert.hpp"

#include <stk_mesh/base/Selector.hpp>
#include <stk_mesh/base/GetEntities.hpp>

#include <vector>

namespace panzer_stk
{

namespace workset_utils
{

template<typename LO, typename GO>
void
convertMeshPartitionToWorkset(const panzer::LocalMeshPartition<LO,GO> & partition,
                              const panzer::WorksetNeeds & needs,
                              panzer::Workset & workset)
{
  workset.setup(partition, needs);

  workset.num_cells = partition.num_owned_cells + partition.num_ghstd_cells + partition.num_virtual_cells;
  workset.subcell_dim = -1;

}

}

Teuchos::RCP<std::vector<panzer::Workset> >  
buildPartitionedWorksets(const panzer_stk::STK_Interface & mesh,
                         const panzer::WorksetDescriptor & description,
                         const panzer::WorksetNeeds & needs)
{

  // Each workset will be represented by a chunk of the mesh
  // These chunks can either be partitioned, or just be a set of cells
  std::vector<panzer::LocalMeshPartition<int,int> > partitions;
  panzer_stk::generateLocalMeshPartitions<int,int>(mesh, description, partitions);

  Teuchos::RCP<std::vector<panzer::Workset> > worksets = Teuchos::rcp(new std::vector<panzer::Workset>());

  for(const auto & partition : partitions){
    worksets->push_back(panzer::Workset());
    workset_utils::convertMeshPartitionToWorkset<int,int>(partition, needs, worksets->back());
  }

  return worksets;

}

}
