/**
* \file
* \brief This module declares the data element structure and adds TlvTree functions.
*
* \author Ingenico
* \author Copyright (c) 2012 Ingenico, 28-32, boulevard de Grenelle,\n
* 75015 Paris, France, All Rights Reserved.
*
* \author Ingenico has intellectual property rights relating to the technology embodied\n
* in this software. In particular, and without limitation, these intellectual property rights may\n
* include one or more patents.\n
* This software is distributed under licenses restricting its use, copying, distribution, and\n
* and decompilation. No part of this software may be reproduced in any form by any means\n
* without prior written authorisation of Ingenico.
**/

#ifndef EPSTOOL_TLVTREE_H_INCLUDED
#define EPSTOOL_TLVTREE_H_INCLUDED

/////////////////////////////////////////////////////////////////
//// Macros & preprocessor definitions //////////////////////////

//// Types //////////////////////////////////////////////////////

typedef unsigned long EPSTOOL_Tag_t;			//!< Type for a tag identifier.
typedef unsigned long EPSTOOL_Length_t;		//!< Type for a length of a tag.
typedef unsigned char *EPSTOOL_Value_t;		//!< Type for a value of a tag.

//! \brief Structure that stores a tag with its length and value.
typedef struct
{
	EPSTOOL_Tag_t tag;			//!< The tag identifier.
	EPSTOOL_Length_t length;		//!< The length of the value.
	EPSTOOL_Value_t value;		//!< A pointer to the value of the tag.
} EPSTOOL_Data_t;

//// Static function definitions ////////////////////////////////

//// Global variables ///////////////////////////////////////////

//// Functions //////////////////////////////////////////////////

void EPSTOOL_TlvTree_Release(TLV_TREE_NODE *node);
void EPSTOOL_TlvTree_ReleaseChildren(TLV_TREE_NODE parent);

int EPSTOOL_TlvTree_CopyChildren(TLV_TREE_NODE tlvTree, const TLV_TREE_NODE hTlvTreeToAdd);

TLV_TREE_NODE EPSTOOL_TlvTree_Add(TLV_TREE_NODE parent, const EPSTOOL_Data_t *data);
void EPSTOOL_TlvTree_GetData(const TLV_TREE_NODE node, EPSTOOL_Data_t *data);
void EPSTOOL_TlvTree_SetData(TLV_TREE_NODE node, const EPSTOOL_Data_t *data);

TLV_TREE_NODE EPSTOOL_TlvTree_GetFirstChildData(const TLV_TREE_NODE parent, EPSTOOL_Data_t *data);
TLV_TREE_NODE EPSTOOL_TlvTree_GetNextData(const TLV_TREE_NODE node, EPSTOOL_Data_t *data);
TLV_TREE_NODE EPSTOOL_TlvTree_FindFirstData(const TLV_TREE_NODE parent, EPSTOOL_Tag_t tag, EPSTOOL_Data_t *data);
TLV_TREE_NODE EPSTOOL_TlvTree_FindNextData(const TLV_TREE_NODE node, EPSTOOL_Tag_t tag, EPSTOOL_Data_t *data);

int EPSTOOL_TlvTree_AddToDelValue(_DEL_ *del, const TLV_TREE_NODE tlvTree);
int EPSTOOL_TlvTree_AddDelValue(TLV_TREE_NODE tlvTree, const _DEL_ *del);
int EPSTOOL_TlvTree_NewFromDel(TLV_TREE_NODE *tlvTree, EPSTOOL_Tag_t tag, const _DEL_ *del);

#endif // EPSTOOL_TLVTREE_H_INCLUDED
