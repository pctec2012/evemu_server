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
    Author:        Bloody.Rabbit
*/

#include "eve-server.h"

#include "manufacturing/Blueprint.h"

/*
 * BlueprintData
 */
BlueprintData::BlueprintData(
    bool _copy,
    uint32 _materialLevel,
    uint32 _productivityLevel,
    int32 _licensedProductionRunsRemaining)
: copy(_copy),
  materialLevel(_materialLevel),
  productivityLevel(_productivityLevel),
  licensedProductionRunsRemaining(_licensedProductionRunsRemaining)
{
}

/*
 * Blueprint
 */
Blueprint::Blueprint(
    uint32 _blueprintID,
    const ItemData &_data,
    // Blueprint stuff:
    const BlueprintData &_bpData)
: InventoryItem(_blueprintID, _data),
  m_copy(_bpData.copy),
  m_materialLevel(_bpData.materialLevel),
  m_productivityLevel(_bpData.productivityLevel),
  m_licensedProductionRunsRemaining(_bpData.licensedProductionRunsRemaining)
{
    // data consistency asserts
    assert(_data.type->getCategoryID() == EVEDB::invCategories::Blueprint);
    m_blueprintType = InvBlueprintType::getBlueprintType(m_type->typeID);
}

BlueprintRef Blueprint::Load(uint32 blueprintID)
{
    return InventoryItem::Load<Blueprint>( blueprintID );
}

BlueprintRef Blueprint::Spawn(ItemData &data, BlueprintData &bpData)
{
    uint32 blueprintID = 0;
    // make sure it's a blueprint type
    if(data.type.get() != nullptr)
    {
        // get the blueprintID
        blueprintID = InventoryItem::_Spawn(data);
        if(blueprintID != 0)
        {
            // insert blueprint entry into DB
            if(!InventoryDB::NewBlueprint(blueprintID, bpData))
            {
                // delete item
                InventoryDB::DeleteItem(blueprintID);
                blueprintID = 0;
            }
        }
    }
    if(blueprintID == 0)
    {
        return BlueprintRef();
    }
    return Blueprint::Load(blueprintID);
}

void Blueprint::Delete()
{
    // delete our blueprint record
    InventoryDB::DeleteBlueprint(m_itemID);
    // redirect to parent
    InventoryItem::Delete();
}

BlueprintRef Blueprint::SplitBlueprint(int32 qty_to_take, bool notify) {
    // split item
    BlueprintRef res = BlueprintRef::StaticCast( InventoryItem::Split( qty_to_take, notify ) );
    if( !res )
        return BlueprintRef();

    // copy our attributes
    res->SetCopy(m_copy);
    res->SetMaterialLevel(m_materialLevel);
    res->SetProductivityLevel(m_productivityLevel);
    res->SetLicensedProductionRunsRemaining(m_licensedProductionRunsRemaining);

    return res;
}

bool Blueprint::Merge(InventoryItemRef to_merge, int32 qty, bool notify) {
    if( !InventoryItem::Merge( to_merge, qty, notify ) )
        return false;
    // do something special? merge material level etc.?
    return true;
}

void Blueprint::SetCopy(bool copy) {
    m_copy = copy;

    SaveBlueprint();
}

void Blueprint::SetMaterialLevel(uint32 materialLevel) {
    m_materialLevel = materialLevel;

    SaveBlueprint();
}

bool Blueprint::AlterMaterialLevel(int32 materialLevelChange) {
    int32 new_material_level = m_materialLevel + materialLevelChange;

    if(new_material_level < 0) {
        _log(ITEM__ERROR, "%s (%u): Tried to remove %u material levels while having %u levels.", m_itemName.c_str(), m_itemID, -materialLevelChange, m_materialLevel);
        return false;
    }

    SetMaterialLevel(new_material_level);
    return true;
}

void Blueprint::SetProductivityLevel(uint32 productivityLevel) {
    m_productivityLevel = productivityLevel;

    SaveBlueprint();
}

bool Blueprint::AlterProductivityLevel(int32 producitvityLevelChange) {
    int32 new_productivity_level = m_productivityLevel + producitvityLevelChange;

    if(new_productivity_level < 0) {
        _log(ITEM__ERROR, "%s (%u): Tried to remove %u productivity levels while having %u levels.", m_itemName.c_str(), m_itemID, -producitvityLevelChange, m_productivityLevel);
        return false;
    }

    SetProductivityLevel(new_productivity_level);
    return true;
}

void Blueprint::SetLicensedProductionRunsRemaining(int32 licensedProductionRunsRemaining) {
    m_licensedProductionRunsRemaining = licensedProductionRunsRemaining;

    SaveBlueprint();
}

void Blueprint::AlterLicensedProductionRunsRemaining(int32 licensedProductionRunsRemainingChange) {
    int32 new_licensed_production_runs_remaining = m_licensedProductionRunsRemaining + licensedProductionRunsRemainingChange;

    SetLicensedProductionRunsRemaining(new_licensed_production_runs_remaining);
}

PyDict *Blueprint::GetBlueprintAttributes() const {
    Rsp_GetBlueprintAttributes rsp;

    // fill in our attribute info
    rsp.blueprintID = itemID();
    rsp.copy = copy() ? 1 : 0;
    rsp.productivityLevel = productivityLevel();
    rsp.materialLevel = materialLevel();
    rsp.licensedProductionRunsRemaining = licensedProductionRunsRemaining();
    rsp.wastageFactor = wasteFactor();

    rsp.productTypeID = m_blueprintType->productTypeID;
    rsp.manufacturingTime = m_blueprintType->productionTime;
    rsp.maxProductionLimit = m_blueprintType->maxProductionLimit;
    rsp.researchMaterialTime = m_blueprintType->researchMaterialTime;
    rsp.researchTechTime = m_blueprintType->researchTechTime;
    rsp.researchProductivityTime = m_blueprintType->researchProductivityTime;
    rsp.researchCopyTime = m_blueprintType->researchCopyTime;

    return(rsp.Encode());
}

void Blueprint::SaveBlueprint() const
{
    _log( ITEM__TRACE, "Saving blueprint %u.", itemID() );

    InventoryDB::SaveBlueprint(
        itemID(),
        BlueprintData(
            copy(),
            materialLevel(),
            productivityLevel(),
            licensedProductionRunsRemaining()
        )
    );
}


