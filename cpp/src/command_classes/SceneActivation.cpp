//-----------------------------------------------------------------------------
//
//	SceneActivation.cpp
//
//	Implementation of the Z-Wave COMMAND_CLASS_SCENE_ACTIVATION
//
//	Copyright (c) 2012 Greg Satz <satz@iranger.com>
//
//	SOFTWARE NOTICE AND LICENSE
//
//	This file is part of OpenZWave.
//
//	OpenZWave is free software: you can redistribute it and/or modify
//	it under the terms of the GNU Lesser General Public License as published
//	by the Free Software Foundation, either version 3 of the License,
//	or (at your option) any later version.
//
//	OpenZWave is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU Lesser General Public License for more details.
//
//	You should have received a copy of the GNU Lesser General Public License
//	along with OpenZWave.  If not, see <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------------------------

#include "command_classes/CommandClasses.h"
#include "command_classes/SceneActivation.h"
#include "Defs.h"
#include "Msg.h"
#include "Node.h"
#include "Driver.h"
#include "Notification.h"
#include "platform/Log.h"

#include "value_classes/ValueByte.h"

using namespace OpenZWave;

enum SceneActivationCmd
{
	SceneActivationCmd_Set				= 0x01
};


//-----------------------------------------------------------------------------
// <SceneActivation::HandleMsg>
// Handle a message from the Z-Wave network
//-----------------------------------------------------------------------------
bool SceneActivation::HandleMsg
(
	uint8 const* _data,
	uint32 const _length,
	uint32 const _instance	// = 1
)
{
	if( SceneActivationCmd_Set == (SceneActivationCmd)_data[0] )
	{
		// Scene Activation Set received so send notification
		char msg[64];
		if( _data[2] == 0 )
			snprintf( msg, sizeof(msg), "now" );
		else if( _data[2] <= 0x7F )
			snprintf( msg, sizeof(msg), "%d seconds", _data[2] );
		else if( _data[2] <= 0xFE )
			snprintf( msg, sizeof(msg), "%d minutes", _data[2] );
		else
			snprintf( msg, sizeof(msg), "via configuration" );
		Log::Write( LogLevel_Info, GetNodeId(), "Received Scene Activation set from node %d: scene id=%d %s. Sending event notification.", GetNodeId(), _data[1], msg );
		Notification* notification = new Notification( Notification::Type_SceneEvent );
		notification->SetHomeAndNodeIds( GetHomeId(), GetNodeId() );
		notification->SetSceneId( _data[1] );
		GetDriver()->QueueNotification( notification );
		return true;
	}

	return false;
}

bool SceneActivation::SetValue
(
	Value const& _value
)
{
	if( ValueID::ValueType_Byte == _value.GetID().GetType() )
	{
		ValueByte const* value = static_cast<ValueByte const*>(&_value);

		Log::Write( LogLevel_Info, GetNodeId(), "SceneActivation::Set - Setting node %d to scene %d", GetNodeId(), value->GetValue() );
		Msg* msg = new Msg( "SceneActivation_Set", GetNodeId(), REQUEST, FUNC_ID_ZW_SEND_DATA, true );
		msg->SetInstance( this, _value.GetID().GetInstance() );
		msg->Append( GetNodeId() );
		msg->Append( 3 );
		msg->Append( GetCommandClassId() );
		msg->Append( SceneActivationCmd_Set );
		msg->Append( value->GetValue() );
		msg->Append( GetDriver()->GetTransmitOptions() );
		GetDriver()->SendMsg( msg, Driver::MsgQueue_Send );
		return true;
	}

	return false;
}

