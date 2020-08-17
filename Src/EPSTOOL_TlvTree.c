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

/////////////////////////////////////////////////////////////////
//// Includes ///////////////////////////////////////////////////

#include "sdk.h"
#include "TlvTree.h"
#include "GTL_Assert.h"
#include "del_lib.h"

#include "EPSTOOL_TlvTree.h"

//// Macros & preprocessor definitions //////////////////////////

//// Types //////////////////////////////////////////////////////

//// Static function definitions ////////////////////////////////

static TLV_TREE_NODE __EPSTOOL_TlvTree_FindData(const TLV_TREE_NODE node, EPSTOOL_Tag_t tag, int first, EPSTOOL_Data_t *data);

//// Global variables ///////////////////////////////////////////

//// Functions //////////////////////////////////////////////////

//! \brief Memory safe release of a TlvTree node (checks for \a NULL pointer).
//! \param[in,out] node Pointer to the node to delete. The output value of \a *node is always \a NULL.
void EPSTOOL_TlvTree_Release(TLV_TREE_NODE *node) {
	// Check input parameters
	if (node != NULL) {
		// Is the node exist ?
		if (*node != NULL) {
			// Release the node
			TlvTree_Release(*node);
			*node = NULL;
		}
	}
}

//! \brief Release all children of a TlvTree node.
//! \param[in,out] parent The TlvTree node to delete all the children.
void EPSTOOL_TlvTree_ReleaseChildren(TLV_TREE_NODE parent) {
	TLV_TREE_NODE node;

	// Check input parameters
	if (parent != NULL) {
		// Remove the first child until there is no child !
		node = TlvTree_GetFirstChild(parent);
		while(node != NULL) {
			TlvTree_Release(node);
			node = TlvTree_GetFirstChild(parent);
		}
	}
}

//! \brief Copy all the children of a TlvTree into another one.
//! \param[in,out] tlvTree The destination TlvTree. It must be not be \a NULL. If the TlvTree already contains some data, they are neither changed nor removed.
//! \param[in] hTlvTreeToAdd The source TlvTree. If \a NULL, the destination TlvTree is not changed and the function returns \a TRUE.
//! \return \a TRUE if the TlvTree has been successfully copied, or \a FALSE if an error occurs (invalid TlvTree, not enough memory, ...).
int EPSTOOL_TlvTree_CopyChildren(TLV_TREE_NODE tlvTree, const TLV_TREE_NODE hTlvTreeToAdd) {
	TLV_TREE_NODE hStartNode;
	TLV_TREE_NODE hNode;
	TLV_TREE_NODE hNodeToAdd;
	TLV_TREE_NODE hNextNode;
	int bResult;
	int bEnd;

	// Check input parameters
	ASSERT(tlvTree != NULL);
	if (hTlvTreeToAdd == NULL) {
		return TRUE;
	}

	hStartNode = hTlvTreeToAdd;
	bResult = TRUE;

	hNode = tlvTree;
	hNodeToAdd = hTlvTreeToAdd;
	// For each child in hTlvTreeToAdd
	do {
		// Go down in the tree
		hNextNode = TlvTree_GetFirstChild(hNodeToAdd);
		if (hNextNode == NULL) {
			if (hNodeToAdd != hStartNode) {
				// Go right in the tree
				hNextNode = TlvTree_GetNext(hNodeToAdd);
				if (hNextNode == NULL) {
					bEnd = FALSE;
					do {
						// Go up in the tree
						hNextNode = TlvTree_GetParent(hNodeToAdd);
						hNode = TlvTree_GetParent(hNode);
						if ((hNextNode != NULL) && (hNextNode != hStartNode)) {
							// Go right in the tree
							hNextNode = TlvTree_GetNext(hNextNode);
							if (hNextNode != NULL) {
								// Found a new node => stop
								hNode = TlvTree_GetParent(hNode);
								bEnd = TRUE;
							} else {
								// Go up in the tree
								hNodeToAdd = TlvTree_GetParent(hNodeToAdd);
							}
						} else {
							// It is the start node => stop
							hNextNode = NULL;
							bEnd = TRUE;
						}
					} while(!bEnd);
				} else {
					// No more brothers => go up
					hNode = TlvTree_GetParent(hNode);
				}
			}
		}

		hNodeToAdd = hNextNode;
		if (hNodeToAdd != NULL) {
			// Add the tag to the destination
			hNode = TlvTree_AddChild(hNode, TlvTree_GetTag(hNodeToAdd), TlvTree_GetData(hNodeToAdd), TlvTree_GetLength(hNodeToAdd));
		}
	} while(hNodeToAdd != NULL);

	return bResult;
}

//! \brief Add a child (\ref EPSTOOL_Data_t structure) to a TlvTree node.
//! \param[in,out] parent The TlvTree node to add a child. It must be not be \a NULL.
//! \param[in] data The TlvTree node to add a child. It must be not be \a NULL.
//! \return The created child node, or \a NULL if an error occurs (not enough memory, corrupted memory, ...).
TLV_TREE_NODE EPSTOOL_TlvTree_Add(TLV_TREE_NODE parent, const EPSTOOL_Data_t *data)
{
	// Check input parameters
	ASSERT(parent != NULL);
	ASSERT(data != NULL);

	// Add a new child
	return TlvTree_AddChild(parent, data->tag, data->value, data->length);
}

//! \brief Retrieve the content of a TlvTree node.
//! \param[in] node The TlvTree node to retrieve information. It must be not be \a NULL.
//! \param[out] data Content of the node (tag, length and value). It must be not be \a NULL.
void EPSTOOL_TlvTree_GetData(const TLV_TREE_NODE node, EPSTOOL_Data_t *data)
{
	// Check input parameters
	ASSERT(node != NULL);
	ASSERT(data != NULL);

	// Get the tag, length and value
	data->tag = TlvTree_GetTag(node);
	data->length = TlvTree_GetLength(node);
	data->value = TlvTree_GetData(node);
}

//! \brief Set the content of a TlvTree node with new values.
//! \param[in,out] node The TlvTree node to set new information. It must be not be \a NULL.
//! \param[in] data The new content of the node (tag, length and value). It must be not be \a NULL.
void EPSTOOL_TlvTree_SetData(TLV_TREE_NODE node, const EPSTOOL_Data_t *data)
{
	// Check input parameters
	ASSERT(node != NULL);
	ASSERT(data != NULL);
	ASSERT((data->length == 0) || (data->value != NULL));

	// Set the tag, length and value
	TlvTree_SetTag(node, data->tag);
	TlvTree_SetData(node, data->value, data->length);
}

//! \brief Retrieve the content of the first child of a TlvTree node.
//! \param[in] parent The TlvTree node to retrieve the first child. It must be not be \a NULL.
//! \param[out] data Content of the first child node (tag, length and value). Can be \a NULL.
//! \return The first child node of \a parent, or \a NULL if there is no child.
TLV_TREE_NODE EPSTOOL_TlvTree_GetFirstChildData(const TLV_TREE_NODE parent, EPSTOOL_Data_t *data)
{
	TLV_TREE_NODE node;

	// Check input parameters
	ASSERT(parent != NULL);

	// Get the first child node
	node = TlvTree_GetFirstChild(parent);
	if ((node != NULL) && (data != NULL))
	{
		// Get the content of the first child
		data->tag = TlvTree_GetTag(node);
		data->length = TlvTree_GetLength(node);
		data->value = TlvTree_GetData(node);
	}

	return node;
}

//! \brief Retrieve the content of the next brother of a TlvTree node.
//! \param[in] node The TlvTree node to retrieve the next brother. It must be not be \a NULL.
//! \param[out] data Content of the next brother node (tag, length and value). Can be \a NULL.
//! \return The next brother node of \a node, or \a NULL if there is no more brother.
TLV_TREE_NODE EPSTOOL_TlvTree_GetNextData(const TLV_TREE_NODE node, EPSTOOL_Data_t *data)
{
	TLV_TREE_NODE nextNode;

	// Check input parameters
	ASSERT(node != NULL);

	// Get the next brother
	nextNode = TlvTree_GetNext(node);
	if ((nextNode != NULL) && (data != NULL))
	{
		// Get the content of the next brother
		data->tag = TlvTree_GetTag(nextNode);
		data->length = TlvTree_GetLength(nextNode);
		data->value = TlvTree_GetData(nextNode);
	}

	return nextNode;
}

//! \brief Search for a given tag in all the children of a TlvTree node.
//! \param[in] node The based TlvTree node for the search. It must be not be \a NULL. If \a first is \a TRUE, it is the parent node and the search is performed in its children.
//! If \a first if \a FALSE, it must be the last found node and the search is performed in its next brothers.
//! \param[in] tag The tag to search for.
//! \param[in] first \a TRUE to search for the first occurrence of \a tag in the children of \a node. \a FALSE to search for the next occurrence of \a tag in the next brothers of \a node.
//! \param[out] data Content of the found node (tag, length and value). Can be \a NULL.
//! \return The found node, or \a NULL if the tag \a tag is not found.
static TLV_TREE_NODE __EPSTOOL_TlvTree_FindData(const TLV_TREE_NODE node, EPSTOOL_Tag_t tag, int first, EPSTOOL_Data_t *data)
{
	TLV_TREE_NODE foundNode;
	int found;

	// Check input parameters
	ASSERT(node != NULL);

	if (first)
	{
		// Get the first node
		foundNode = TlvTree_GetFirstChild(node);
	}
	else
	{
		// Get the next node
		foundNode = TlvTree_GetNext(node);
	}

	found = FALSE;
	while((!found) && (foundNode != NULL))
	{
		// Is the node is the one we search for ?
		if (TlvTree_GetTag(foundNode) == tag)
		{
			// Yes! We found it
			found = TRUE;
		}
		else
		{
			// No! Get the next brother
			foundNode = TlvTree_GetNext(foundNode);
		}
	}

	if ((foundNode != NULL) && (data != NULL))
	{
		// Get the content of the next brother
		data->tag = TlvTree_GetTag(foundNode);
		data->length = TlvTree_GetLength(foundNode);
		data->value = TlvTree_GetData(foundNode);
	}

	return foundNode;
}

//! \brief Search for a given tag in all the children of a TlvTree node.
//! \param[in] parent The based TlvTree node for the search. It must be not be \a NULL
//! \param[in] tag The tag to search for.
//! \param[out] data Content of the found node (tag, length and value). Can be \a NULL.
//! \return The found node, or \a NULL if the tag \a tag is not found.
TLV_TREE_NODE EPSTOOL_TlvTree_FindFirstData(const TLV_TREE_NODE parent, EPSTOOL_Tag_t tag, EPSTOOL_Data_t *data)
{
	// Check input parameters
	ASSERT(parent != NULL);
	return __EPSTOOL_TlvTree_FindData(parent, tag, TRUE, data);
}

//! \brief Search for a given tag in all the next brothers of a TlvTree node.
//! \param[in] node The based TlvTree node for the search. It must be not be \a NULL. It is usually the last found node. The search is performed in its next brothers.
//! \param[in] tag The tag to search for.
//! \param[out] data Content of the found node (tag, length and value). Can be \a NULL.
//! \return The found node, or \a NULL if the tag \a tag is not found.
TLV_TREE_NODE EPSTOOL_TlvTree_FindNextData(const TLV_TREE_NODE node, EPSTOOL_Tag_t tag, EPSTOOL_Data_t *data)
{
	// Check input parameters
	ASSERT(node != NULL);
	return __EPSTOOL_TlvTree_FindData(node, tag, FALSE, data);
}

//! \brief Add all the tags from a TlvTree into a \a _DEL_.
//! \param[in] del The destination \a _DEL_. It must be not be \a NULL.
//! \param[in] tlvTree The source TlvTree. It must be not be \a NULL.
//! \return \a TRUE if the TlvTree has been successfully copied, or \a FALSE if an error occurs (invalid TlvTree, not enough memory, ...).
int EPSTOOL_TlvTree_AddToDelValue(_DEL_ *del, const TLV_TREE_NODE tlvTree) {
	TLV_TREE_NODE hNode;
	int bResult;
	DataElement DataElt;

	// Check input parameters
	ASSERT(del != NULL);
	ASSERT(tlvTree != NULL);

	// For each tag in the TlvTree
	bResult = TRUE;
	hNode = TlvTree_GetFirstChild(tlvTree);
	while(hNode != NULL) {
		// Add the tag to the DEL
		DataElt.tag = TlvTree_GetTag(hNode);
		DataElt.length = TlvTree_GetLength(hNode);
		DataElt.ptValue = TlvTree_GetData(hNode);
		if (DelValue_AddDataElement(del, &DataElt) != DEL_OK) {
			bResult = FALSE;
		}

		// Next tag
		hNode = TlvTree_GetNext(hNode);
	}

	return bResult;
}

//! \brief Add all the tags from a \a _DEL_ into a TlvTree.
//! \param[in] tlvTree The destination TlvTree. It must be not be \a NULL.
//! \param[in] del The source \a _DEL_. It must be not be \a NULL.
//! \return \a TRUE if the _DEL_ has been successfully copied, or \a FALSE if an error occurs (invalid TlvTree, not enough memory, ...).
int EPSTOOL_TlvTree_AddDelValue(TLV_TREE_NODE tlvTree, const _DEL_ *del)
{
	int nNumOfTags;
	int nIndex;
	_DataElement_* pDataElement;
	int bResult;

	// Check input parameters
	ASSERT(tlvTree != NULL);
	ASSERT(del != NULL);

	// Get the number of tags in the DEL
	bResult = TRUE;
	nNumOfTags = _DEL_GetCount(del);

	// For each tag in the DEL
	for(nIndex = 1; nIndex <= nNumOfTags; nIndex++)
	{
		// Get the tag from the DEL
		pDataElement = _DEL_GetDataElement((_DEL_*)del, nIndex);
		if (pDataElement != NULL)
		{
			// Add the tag to the TlvTree
			if (TlvTree_AddChild(tlvTree, pDataElement->tag, pDataElement->ptValue, pDataElement->length) == NULL)
			{
				bResult = FALSE;
			}
		}
	}

	return bResult;
}

//! \brief Create anew TlvTree and copy all the tags from a \a _DEL_ into.
//! \param[in,out] tlvTree A pointer to a variable that will receive the new TlvTree node. It must be not be \a NULL.
//! \param[in] tag The tag of the new TlvTree.
//! \param[in] del The source \a _DEL_. It must be not be \a NULL.
//! \return \a TRUE if the TlvTree has been successfully created and initialised with the \a del tags, or \a FALSE if an error occurs (not enough memory, ...).
int EPSTOOL_TlvTree_NewFromDel(TLV_TREE_NODE *tlvTree, EPSTOOL_Tag_t tag, const _DEL_ *del)
{
	// Check input parameters
	ASSERT(tlvTree != NULL);
	ASSERT(del != NULL);

	// Create the new TlvTree
	*tlvTree = TlvTree_New(tag);
	if (*tlvTree == NULL)
	{
		return FALSE;
	}

	// Copy the _DEL_
	if (!EPSTOOL_TlvTree_AddDelValue(*tlvTree, del))
	{
		EPSTOOL_TlvTree_Release(tlvTree);
		return FALSE;
	}

	return TRUE;
}

void emvStop(void){

}
