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
    Author:        Zhur, Aknor Jaden
*/


#include "eve-server.h"

#include "PyServiceCD.h"
#include "chat/LookupService.h"
#include "chat/LSCDB.h"

/*
class LookupSvcBound
: public LookupSvcObject {
public:

    class Dispatcher
    : public PyCallableDispatcher<LookupSvcBound> {
    public:
        Dispatcher(LookupSvcBound *c)
        : PyCallableDispatcher<LookupSvcBound>(c) {}
    };

    LookupSvcBound(LookupSvcDB *db)
    : PyBoundObject(mgr, "LookupSvcBound", new Dispatcher(this)),
      m_db(db)
    {
        PyCallable_REG_CALL(LookupSvcBound, )
        PyCallable_REG_CALL(LookupSvcBound, )
    }
    virtual ~LookupSvcBound() {}
    virtual void Release() {
        //I hate this statement
        delete this;
    }

    PyCallable_DECL_CALL()
    PyCallable_DECL_CALL()

protected:
    LookupSvcDB *const m_db;

};
*/

PyCallable_Make_InnerDispatcher(LookupService)

LookupService::LookupService()
: PyService("lookupSvc", new Dispatcher(this))
{
    PyCallable_REG_CALL(LookupService, LookupCharacters)
    PyCallable_REG_CALL(LookupService, LookupOwners)
    PyCallable_REG_CALL(LookupService, LookupPlayerCharacters)
    PyCallable_REG_CALL(LookupService, LookupCorporations)
    PyCallable_REG_CALL(LookupService, LookupFactions)
    PyCallable_REG_CALL(LookupService, LookupCorporationTickers)
    PyCallable_REG_CALL(LookupService, LookupStations)
    PyCallable_REG_CALL(LookupService, LookupKnownLocationsByGroup)

    PyCallable_REG_CALL(LookupService, LookupEvePlayerCharacters)
    //PyCallable_REG_CALL(LookupService, )
}

LookupService::~LookupService() {
}


/*
PyBoundObject* LookupService::_CreateBoundObject( Client* c, const PyRep* bind_args )
{
    _log( CLIENT__MESSAGE, "LookupService bind request for:" );
    bind_args->Dump( CLIENT__MESSAGE, "    " );

    return new LookupSvcBound( m_manager, &m_db );
}*/

PyResult LookupService::Handle_LookupEvePlayerCharacters(PyCallArgs& call) {
    Call_LookupStringInt args;
    if (!args.Decode(&call.tuple)) {
        codelog(SERVICE__ERROR, "Wrong incoming param in LookupEvePlayerCharacters");
        return NULL;
    }

    return LSCDB::LookupPlayerChars(args.searchString.c_str(), args.searchOption ? true : false);
}

PyResult LookupService::Handle_LookupCharacters(PyCallArgs &call) {
    Call_LookupStringInt args;
    if (!args.Decode(&call.tuple)) {
        codelog(SERVICE__ERROR, "Wrong incoming param in LookupCharacters");
        return NULL;
    }

    return LSCDB::LookupChars(args.searchString.c_str(), args.searchOption ? true : false);
}

//LookupOwners
PyResult LookupService::Handle_LookupOwners(PyCallArgs &call) {
    Call_LookupStringInt args;
    if (!args.Decode(&call.tuple)) {
        codelog(SERVICE__ERROR, "Wrong incoming param in LookupOwners");
        return NULL;
    }

    return LSCDB::LookupOwners(args.searchString.c_str(), args.searchOption ? true : false);
}

PyResult LookupService::Handle_LookupPlayerCharacters(PyCallArgs &call) {
    Call_LookupStringInt args;
    if (!args.Decode(&call.tuple)) {
        codelog(SERVICE__ERROR, "Wrong incoming param in LookupPlayerCharacters");
        return NULL;
    }

    return LSCDB::LookupPlayerChars(args.searchString.c_str(), false);
}
PyResult LookupService::Handle_LookupCorporations(PyCallArgs &call) {
    Call_LookupStringInt args;
    if (!args.Decode(&call.tuple)) {
        codelog(SERVICE__ERROR, "Wrong incoming param in LookupCorporations");
        return NULL;
    }

    return LSCDB::LookupCorporations(args.searchString);
}
PyResult LookupService::Handle_LookupFactions(PyCallArgs &call) {
    Call_LookupStringInt args;
    if (!args.Decode(&call.tuple)) {
        codelog(SERVICE__ERROR, "Wrong incoming param in LookupCorporations");
        return NULL;
    }

    return LSCDB::LookupFactions(args.searchString);
}
PyResult LookupService::Handle_LookupCorporationTickers(PyCallArgs &call) {
    Call_LookupStringInt args;
    if (!args.Decode(&call.tuple)) {
        codelog(SERVICE__ERROR, "Wrong incoming param in LookupCorporations");
        return NULL;
    }

    return LSCDB::LookupCorporationTickers(args.searchString);
}
PyResult LookupService::Handle_LookupStations(PyCallArgs &call) {
    Call_LookupStringInt args;
    if (!args.Decode(&call.tuple)) {
        codelog(SERVICE__ERROR, "Wrong incoming param in LookupCorporations");
        return NULL;
    }

    return LSCDB::LookupStations(args.searchString);
}
// Asteroids, constellations and regions should be injected into the entity table...
PyResult LookupService::Handle_LookupKnownLocationsByGroup(PyCallArgs &call) {
    Call_LookupIntString args;
    if (!args.Decode(&call.tuple)) {
        codelog(SERVICE__ERROR, "Wrong incoming param in LookupCorporations");
        return NULL;
    }

    return LSCDB::LookupKnownLocationsByGroup(args.searchString, args.searchOption);
}
























