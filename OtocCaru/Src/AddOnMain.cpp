#include "APIEnvir.h"
#include "ACAPinc.h"

#include "ResourceIds.hpp"
#include "RS.hpp"
#include "UniString.hpp"

static const GSResID AddOnInfoID			= ID_ADDON_INFO;
	static const Int32 AddOnNameID			= 1;
	static const Int32 AddOnDescriptionID	= 2;

static const short AddOnMenuID				= ID_ADDON_MENU;
	static const Int32 AddOnCommandID		= 1;

static const GSResID MessagesID				= ID_ADDON_MSG;
	static const Int32 MsgUndoNameID		= 1;
	static const Int32 MsgNoLinesID			= 2;
	static const Int32 MsgDoneID			= 3;


static GS::UniString GetMessage (Int32 index)
{
	GS::UniString text;
	RSGetIndString (&text, MessagesID, index, ACAPI_GetOwnResModule ());
	return text;
}


// -----------------------------------------------------------------------------
// Obrátí směr jedné čáry (prohodí počáteční a koncový bod)
// -----------------------------------------------------------------------------
static bool ReverseLine (const API_Guid& guid)
{
	API_Element element {};
	element.header.guid = guid;

	if (ACAPI_Element_Get (&element) != NoError)
		return false;

	if (element.header.type.typeID != API_LineID)
		return false;

	const API_Coord oldBegin = element.line.begC;
	element.line.begC = element.line.endC;
	element.line.endC = oldBegin;

	API_Element mask {};
	ACAPI_ELEMENT_MASK_CLEAR (mask);
	ACAPI_ELEMENT_MASK_SET (mask, API_LineType, begC);
	ACAPI_ELEMENT_MASK_SET (mask, API_LineType, endC);

	return ACAPI_Element_Change (&element, &mask, nullptr, 0, true) == NoError;
}


// -----------------------------------------------------------------------------
// Příkaz z menu: obrátí směr všech vybraných čar
// -----------------------------------------------------------------------------
static void ReverseSelectedLines ()
{
	API_SelectionInfo		selectionInfo {};
	GS::Array<API_Neig>		selNeigs;

	const GSErrCode selErr = ACAPI_Selection_Get (&selectionInfo, &selNeigs, true);
	BMKillHandle ((GSHandle*) &selectionInfo.marquee.coords);

	GS::Array<API_Guid> lineGuids;
	if (selErr == NoError) {
		for (const API_Neig& neig : selNeigs) {
			API_ElemType elemType;
			if (ACAPI_Element_NeigIDToElemType (neig.neigID, elemType) == NoError && elemType.typeID == API_LineID)
				lineGuids.Push (neig.guid);
		}
	}

	if (lineGuids.IsEmpty ()) {
		ACAPI_WriteReport (GetMessage (MsgNoLinesID), true);
		return;
	}

	Int32 reversedCount = 0;
	ACAPI_CallUndoableCommand (GetMessage (MsgUndoNameID), [&] () -> GSErrCode {
		for (const API_Guid& guid : lineGuids) {
			if (ReverseLine (guid))
				++reversedCount;
		}
		return NoError;
	});

	ACAPI_WriteReport (GetMessage (MsgDoneID), false, reversedCount);
}


static GSErrCode MenuCommandHandler (const API_MenuParams *menuParams)
{
	switch (menuParams->menuItemRef.menuResID) {
		case AddOnMenuID:
			switch (menuParams->menuItemRef.itemIndex) {
				case AddOnCommandID:
					ReverseSelectedLines ();
					break;
			}
			break;
	}
	return NoError;
}

#ifdef ServerMainVers_2700
#if defined(macintosh)

bool CCALL ResourceTypesCallback (GSResType resType, GSResModule /*resModule*/, void *userData)
{
	constexpr GSResType libPartType = 'FLDR';
	if (resType == libPartType) {
		bool* hasLibPart = static_cast<bool*> (userData);
		*hasLibPart = true;
		return false; // stop enumeration
	}
	return true; // continue enumeration
}

#else

bool CCALL ResourceTypesCallback (const WCHAR* resType, GSResModule /*resModule*/, void *userData)
{
	constexpr auto libPartType = "FLDR";
	if (GS::UniString (resType) == libPartType) {
		bool* hasLibPart = static_cast<bool*> (userData);
		*hasLibPart = true;
		return false; // stop enumeration
	}
	return true; // continue enumeration
}
#endif

static bool HasBuiltInLibPart ()
{
	bool hasLibPart = false;
	RSEnumResourceTypes (ResourceTypesCallback, &hasLibPart, ACAPI_GetOwnResModule ());

	return hasLibPart;
}

#endif

API_AddonType CheckEnvironment (API_EnvirParams* envir)
{
	RSGetIndString (&envir->addOnInfo.name, AddOnInfoID, AddOnNameID, ACAPI_GetOwnResModule ());
	RSGetIndString (&envir->addOnInfo.description, AddOnInfoID, AddOnDescriptionID, ACAPI_GetOwnResModule ());

	return APIAddon_Normal;
}

GSErrCode RegisterInterface (void)
{
#ifdef ServerMainVers_2700
	GSErrCode err = ACAPI_MenuItem_RegisterMenu (AddOnMenuID, 0, MenuCode_Tools, MenuFlag_Default);
	if (err == NoError && HasBuiltInLibPart ())
		err = ACAPI_AddOnIntegration_RegisterBuiltInLibrary ();

	return err;
#else
	return ACAPI_Register_Menu (AddOnMenuID, 0, MenuCode_Tools, MenuFlag_Default);
#endif
}

GSErrCode Initialize (void)
{
#ifdef ServerMainVers_2700
	return ACAPI_MenuItem_InstallMenuHandler (AddOnMenuID, MenuCommandHandler);
#else
	return ACAPI_Install_MenuHandler (AddOnMenuID, MenuCommandHandler);
#endif
}

GSErrCode FreeData (void)
{
	return NoError;
}
