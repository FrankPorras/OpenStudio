/***********************************************************************************************************************
 *  OpenStudio(R), Copyright (c) 2008-2018, Alliance for Sustainable Energy, LLC. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 *  following conditions are met:
 *
 *  (1) Redistributions of source code must retain the above copyright notice, this list of conditions and the following
 *  disclaimer.
 *
 *  (2) Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
 *  following disclaimer in the documentation and/or other materials provided with the distribution.
 *
 *  (3) Neither the name of the copyright holder nor the names of any contributors may be used to endorse or promote
 *  products derived from this software without specific prior written permission from the respective party.
 *
 *  (4) Other than as required in clauses (1) and (2), distributions in any form of modifications or other derivative
 *  works may not use the "OpenStudio" trademark, "OS", "os", or any other confusingly similar designation without
 *  specific prior written permission from Alliance for Sustainable Energy, LLC.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER, THE UNITED STATES GOVERNMENT, OR ANY CONTRIBUTORS BE LIABLE FOR
 *  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 *  AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 **********************************************************************************************************************/

#include <gtest/gtest.h>
#include "ModelFixture.hpp"
#include "../AirTerminalSingleDuctInletSideMixer.hpp"
#include "../AirTerminalSingleDuctInletSideMixer_Impl.hpp"
#include "../AirLoopHVAC.hpp"
#include "../PlantLoop.hpp"
#include "../Node.hpp"
#include "../Node_Impl.hpp"
#include "../Schedule.hpp"
#include "../ScheduleCompact.hpp"
#include "../AirLoopHVACZoneSplitter.hpp"
#include "../ThermalZone.hpp"
#include "../DesignSpecificationOutdoorAir.hpp"

using namespace openstudio::model;

TEST_F(ModelFixture, AirTerminalSingleDuctInletSideMixer_DefaultConstructor)
{
  ::testing::FLAGS_gtest_death_test_style = "threadsafe";

  ASSERT_EXIT ( 
  {  
    Model model;
    AirTerminalSingleDuctInletSideMixer testObject = AirTerminalSingleDuctInletSideMixer(model);
    exit(0); 
  } ,
    ::testing::ExitedWithCode(0), "" );
}

TEST_F(ModelFixture,AirTerminalSingleDuctInletSideMixer_addToNode) {
  Model m; 
  AirTerminalSingleDuctInletSideMixer testObject(m);

  AirLoopHVAC airLoop(m);

  Node supplyOutletNode = airLoop.supplyOutletNode();

  EXPECT_FALSE(testObject.addToNode(supplyOutletNode));
  EXPECT_EQ( (unsigned)2, airLoop.supplyComponents().size() );

  Node inletNode = airLoop.zoneSplitter().lastOutletModelObject()->cast<Node>();

  EXPECT_TRUE(testObject.addToNode(inletNode));
  EXPECT_EQ((unsigned)7, airLoop.demandComponents().size());

  PlantLoop plantLoop(m);
  supplyOutletNode = plantLoop.supplyOutletNode();
  EXPECT_FALSE(testObject.addToNode(supplyOutletNode));
  EXPECT_EQ( (unsigned)5, plantLoop.supplyComponents().size() );

  Node demandOutletNode = plantLoop.demandOutletNode();
  EXPECT_FALSE(testObject.addToNode(demandOutletNode));
  EXPECT_EQ( (unsigned)5, plantLoop.demandComponents().size() );

  AirTerminalSingleDuctInletSideMixer testObjectClone = testObject.clone(m).cast<AirTerminalSingleDuctInletSideMixer>();
  inletNode = airLoop.zoneSplitter().lastOutletModelObject()->cast<Node>();

  EXPECT_FALSE(testObjectClone.addToNode(inletNode));
  EXPECT_TRUE(airLoop.addBranchForHVACComponent(testObjectClone));
  EXPECT_EQ( (unsigned)10, airLoop.demandComponents().size() );
}

TEST_F(ModelFixture, AirTerminalSingleDuctInletSideMixer_AddToNodeWithThermalZone)
{
  Model model;
  AirTerminalSingleDuctInletSideMixer testObject = AirTerminalSingleDuctInletSideMixer(model);

  AirLoopHVAC airLoop(model);
  ThermalZone thermalZone(model);

  airLoop.addBranchForZone(thermalZone);
  Node inletNode = airLoop.zoneSplitter().lastOutletModelObject()->cast<Node>();

  EXPECT_TRUE(testObject.addToNode(inletNode));
  EXPECT_EQ((unsigned)9, airLoop.demandComponents().size());
  
  EXPECT_TRUE(testObject.inletPort());
  EXPECT_TRUE(testObject.outletPort());   
}

TEST_F(ModelFixture, AirTerminalSingleDuctInletSideMixer_AddAirTerminalToPlantLoopAddDemandBranchForComponent)
{
  Model model;
  AirTerminalSingleDuctInletSideMixer testObject = AirTerminalSingleDuctInletSideMixer(model);

  AirLoopHVAC airLoop(model);
  ThermalZone thermalZone(model);
  PlantLoop plantLoop(model);

  airLoop.addBranchForZone(thermalZone);
  Node inletNode = airLoop.zoneSplitter().lastOutletModelObject()->cast<Node>();

  testObject.addToNode(inletNode);

  EXPECT_FALSE(plantLoop.addDemandBranchForComponent(testObject));
  EXPECT_EQ((unsigned)9, airLoop.demandComponents().size());
  EXPECT_EQ((unsigned)5, plantLoop.demandComponents().size());
  
  EXPECT_TRUE(testObject.inletPort());
  EXPECT_TRUE(testObject.outletPort());   
}

TEST_F(ModelFixture, AirTerminalSingleDuctInletSideMixer_AddObjectByAirLoopAddBranchForZoneWithThermalZone)
{
  Model model;
  AirTerminalSingleDuctInletSideMixer testObject = AirTerminalSingleDuctInletSideMixer(model);

  AirLoopHVAC airLoop(model);
  ThermalZone thermalZone(model);

  EXPECT_TRUE(airLoop.addBranchForZone(thermalZone, testObject));
  EXPECT_EQ((unsigned)9, airLoop.demandComponents().size());
  
  EXPECT_TRUE(testObject.inletPort());
  EXPECT_TRUE(testObject.outletPort());   
}

TEST_F(ModelFixture, AirTerminalSingleDuctInletSideMixer_AddObjectByAirLoopAddBranchForHVACComponent)
{
  Model model;
  AirTerminalSingleDuctInletSideMixer testObject = AirTerminalSingleDuctInletSideMixer(model);

  AirLoopHVAC airLoop(model);

  EXPECT_TRUE(airLoop.addBranchForHVACComponent(testObject));
  EXPECT_EQ((unsigned)7, airLoop.demandComponents().size());
  
  EXPECT_TRUE(testObject.inletPort());
  EXPECT_TRUE(testObject.outletPort());   
}

TEST_F(ModelFixture, AirTerminalSingleDuctInletSideMixer_AddObjectByAirLoopAddBranchForHVACComponentWithThermalZone)
{
  Model model;
  AirTerminalSingleDuctInletSideMixer testObject = AirTerminalSingleDuctInletSideMixer(model);

  AirLoopHVAC airLoop(model);
  ThermalZone thermalZone(model);

  airLoop.addBranchForZone(thermalZone);

  EXPECT_TRUE(airLoop.addBranchForHVACComponent(testObject));
  EXPECT_EQ((unsigned)10, airLoop.demandComponents().size());
  
  EXPECT_TRUE(testObject.inletPort());
  EXPECT_TRUE(testObject.outletPort());   
}

TEST_F(ModelFixture, AirTerminalSingleDuctInletSideMixer_AddToNodeTwoSameObjects)
{
  Model model;
  AirTerminalSingleDuctInletSideMixer testObject = AirTerminalSingleDuctInletSideMixer(model);

  AirLoopHVAC airLoop(model);

  Node inletNode = airLoop.zoneSplitter().lastOutletModelObject()->cast<Node>();
  testObject.addToNode(inletNode);
  inletNode = airLoop.zoneSplitter().lastOutletModelObject()->cast<Node>();
  EXPECT_FALSE(testObject.addToNode(inletNode));
  
  EXPECT_TRUE(testObject.inletPort());
  EXPECT_TRUE(testObject.outletPort());   
}

TEST_F(ModelFixture, AirTerminalSingleDuctInletSideMixer_IsRemovable)
{
  Model model;
  AirTerminalSingleDuctInletSideMixer testObject = AirTerminalSingleDuctInletSideMixer(model);

  EXPECT_TRUE(testObject.isRemovable());
}

TEST_F(ModelFixture, AirTerminalSingleDuctInletSideMixer_Remove)
{
  Model model;
  AirTerminalSingleDuctInletSideMixer testObject = AirTerminalSingleDuctInletSideMixer(model);

  AirLoopHVAC airLoop(model);

  Node inletNode = airLoop.zoneSplitter().lastOutletModelObject()->cast<Node>();
  testObject.addToNode(inletNode);

  EXPECT_EQ((unsigned)7, airLoop.demandComponents().size());

  testObject.remove();
  EXPECT_EQ((unsigned)5, airLoop.demandComponents().size());
}

TEST_F(ModelFixture, AirTerminalSingleDuctInletSideMixer_RemoveObjectWithThermalZone)
{
  Model model;
  AirTerminalSingleDuctInletSideMixer testObject = AirTerminalSingleDuctInletSideMixer(model);

  AirLoopHVAC airLoop(model);
  ThermalZone thermalZone(model);

  airLoop.addBranchForZone(thermalZone);

  Node inletNode = airLoop.zoneSplitter().lastOutletModelObject()->cast<Node>();
  testObject.addToNode(inletNode);

  EXPECT_EQ((unsigned)9, airLoop.demandComponents().size());

  testObject.remove();
  EXPECT_EQ((unsigned)7, airLoop.demandComponents().size());
}


