/*
    ------------------------------------------------------------------------------------
    LICENSE:
    ------------------------------------------------------------------------------------
    This file is part of EVEmu: EVE Online Server Emulator
    Copyright 2006 - 2016 The EVEmu Team
    For the latest information visit http://evemu.org
    ------------------------------------------------------------------------------------
    This program is free software; you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License as published by the Free Software
    Foundation; either version 2 of the License, or (at your option) any later
    version.

    This program is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
    this program; if not, write to the Free Software Foundation, Inc., 59 Temple
    Place - Suite 330, Boston, MA 02111-1307, USA, or go to
    http://www.gnu.org/copyleft/lesser.txt.
    ------------------------------------------------------------------------------------
    Author:        Luck
*/


#include "eve-server.h"

#include "ship/modules/ActiveModules.h"

ActiveModule::ActiveModule(InventoryItemRef item, ShipRef ship)
: GenericModule(item, ship),
 m_timer(0)
{
    m_Stop = false;

	m_chargeRef = InventoryItemRef();		// Ensure ref is NULL
	m_chargeLoaded = false;
}

ActiveModule::~ActiveModule()
{
}

void ActiveModule::Offline()
{
    m_Item->PutOffline();
}

void ActiveModule::Online()
{
    m_Item->PutOnline();
}

void ActiveModule::Activate(SystemEntity * targetEntity)
{
	//This will be handled by the Module class itself (eg. Afterburner.cpp)
}

void ActiveModule::Deactivate()
{
	//This will be handled by the Module class itself (eg. Afterburner.cpp)
}

void ActiveModule::Load(InventoryItemRef charge)
{
	m_chargeRef = charge;
	m_chargeLoaded = true;
}

void ActiveModule::Unload()
{
	m_chargeRef = InventoryItemRef();		// Ensure ref is NULL
	m_chargeLoaded = false;
}

/****************************************************
    A little note about the timer:
        Timer.Check() has two functions:
            1. It checks if the timer has expired out
            2. It subtracts from the start time
    Don't be fooled by it's name because if you don't
    call it in a loop, you won't get the time moving.
 *****************************************************/

void ActiveModule::Process()
{
    //check if we have signal to stop the cycle
    if (!m_Stop)
    {
        //check if the timer expired & subtract time
        if (m_timer.Check())
        {
            if (ShouldProcessActiveCycle())
            {
                //time passed and we can drain cap and make/maintain changes to the attributes
                SysLog::Debug("ActiveModule", "Cycle finished, processing...");
                ProcessActiveCycle();
            }
            else
            {
                //m_Item->SetActive(false, 1253, 0, false);
                m_Stop = true;
            }
        }
        //else
        //{
        //	//wait for time to run out and send deactivate to client
        //	m_timer.Disable();
        //	//m_Item->SetActive(false, 1253, 0, false);
        //	StopCycle();
        //}
    }
    else
    {
        if (m_timer.Check())
        {
            //wait for time to run out and send deactivate to client
            m_timer.Disable();
            //m_Item->SetActive(false, 1253, 0, false);
            StopCycle();
        }
    }

}

void ActiveModule::ActivateCycle()
{
    m_Stop = false;
    if (HasAttribute(AttrDuration))
    {
        m_timer.Start(GetAttribute(AttrDuration).get_int());
        //DoCycle();	// Do initial cycle immediately while we start timer
    }
    else
    {
        if (HasAttribute(AttrSpeed))
        {
            m_timer.Start(GetAttribute(AttrSpeed).get_int());
            //DoCycle();	// Do initial cycle immediately while we start timer
        }
        else
            SysLog::Error("ActiveModule::ActivateCycle()", "ERROR! ActiveModule '%s' (id %u) has neither AttrSpeed nor AttrDuration! No way to process time-based cycle!", getItem()->itemName().c_str(), getItem()->itemID());
    }
}

void ActiveModule::DeactivateCycle()
{
    m_Stop = true;
}

void ActiveModule::AbortCycle()
{
    // Immediately stop active cycle for things such as target destroyed or left bubble, or asteroid emptied and removed from space:
    m_Stop = true;
    m_timer.Disable();
    StopCycle(true);
}

//timing and verification function

bool ActiveModule::ShouldProcessActiveCycle()
{
    //first, check if we have been told to deactivate
    if (m_Stop)
        return false;

    //check that we have enough capacitor avaiable
    if (m_Ship->GetAttribute(AttrCharge) > GetAttribute(AttrCapacitorNeed))
    {
        //having enough capacitor to activate the module
        return true;
    }
    else
    {
        return false;
    }
}

void ActiveModule::ProcessActiveCycle()
{
    //TO-DO: Check to see if capacitor is drained at activation or after the cycle

    //check for stop signal
    if (m_Stop)
        return;

    //else consume capacitor
    EvilNumber capCapacity = m_Ship->GetAttribute(AttrCharge);
    EvilNumber capNeed = GetAttribute(AttrCapacitorNeed);
    capCapacity -= capNeed;

    m_Ship->SetAttribute(AttrCharge, capCapacity);

    //then check if we are targeting another ship or not and apply attribute changes
    //maybe we can have a check for modules that repeat the same attributes so we
    //send the changes just once at activation and at deactivation

    //--pseudocode--
    //if(target != self)
    //	m_ShipAttrComp->ModifyTargetShipAttribute();
    //else
    //	m_ShipAttrComp->ModifyShipAttribute();
    DoCycle();
}

void ActiveModule::ProcessDeactivateCycle() {
    //check to see who is the target

    //--pseudocode--
    //if(target != self)
    //	m_ShipAttrComp->ModifyTargetShipAttribute();
    //else
    //	m_ShipAttrComp->ModifyShipAttribute();
}

double ActiveModule::GetRemainingCycleTimeMS()
{
    return (double) (m_timer.GetRemainingTime());
}

double ActiveModule::GetElapsedCycleTimeMS()
{
    return (double) (m_timer.GetElapsedTime());
}

double ActiveModule::GetTotalCycleTimeMS()
{
    return (double) (m_timer.GetTimerTime());
}
