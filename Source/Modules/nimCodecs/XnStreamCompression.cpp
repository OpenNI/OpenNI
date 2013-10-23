/*****************************************************************************
*                                                                            *
*  OpenNI 1.x Alpha                                                          *
*  Copyright (C) 2012 PrimeSense Ltd.                                        *
*                                                                            *
*  This file is part of OpenNI.                                              *
*                                                                            *
*  Licensed under the Apache License, Version 2.0 (the "License");           *
*  you may not use this file except in compliance with the License.          *
*  You may obtain a copy of the License at                                   *
*                                                                            *
*      http://www.apache.org/licenses/LICENSE-2.0                            *
*                                                                            *
*  Unless required by applicable law or agreed to in writing, software       *
*  distributed under the License is distributed on an "AS IS" BASIS,         *
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  *
*  See the License for the specific language governing permissions and       *
*  limitations under the License.                                            *
*                                                                            *
*****************************************************************************/
//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnStreamCompression.h"
#include <XnStatus.h>
#include <jerror.h>
#include <XnLog.h>

#define XN_MASK_STREAM_COMPRESSION "xnStreamCompression"

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnStatus XnStreamCompressDepth16Z(const XnUInt16* pInput, const XnUInt32 nInputSize, XnUInt8* pOutput, XnUInt32* pnOutputSize)
{
	// Local function variables
	const XnUInt16* pInputEnd = pInput + (nInputSize / sizeof(XnUInt16));
	XnUInt8* pOrigOutput = pOutput;
	XnUInt16 nCurrValue = 0;
	XnUInt16 nLastValue = 0;
	XnUInt16 nAbsDiffValue = 0;
	XnInt16 nDiffValue = 0;
	XnUInt8 cOutStage = 0;
	XnUInt8 cOutChar = 0;
	XnUInt8 cZeroCounter = 0;

	// Note: this function does not make sure it stay within the output memory boundaries!

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(pInput);
	XN_VALIDATE_INPUT_PTR(pOutput);
	XN_VALIDATE_INPUT_PTR(pnOutputSize);

	if (nInputSize == 0)
	{
		*pnOutputSize = 0;
		return XN_STATUS_OK;
	}

	// Encode the data...
	nLastValue = *pInput;
	*(XnUInt16*)pOutput = nLastValue;
	pInput++;
	pOutput+=2;

	while (pInput != pInputEnd)
	{	
		nCurrValue = *pInput;

		nDiffValue = (nLastValue - nCurrValue);
		nAbsDiffValue = (XnUInt16)abs(nDiffValue);

		if (nAbsDiffValue <= 6)
		{
			nDiffValue += 6;

			if (cOutStage == 0)
			{
				cOutChar = (XnUInt8)(nDiffValue << 4);

				cOutStage = 1;
			}
			else
			{
				cOutChar += (XnUInt8)nDiffValue;

				if (cOutChar == 0x66)
				{
					cZeroCounter++;

					if (cZeroCounter == 15)
					{
						*pOutput = 0xEF;
						pOutput++;

						cZeroCounter = 0;
					}
				}
				else
				{
					if (cZeroCounter != 0)
					{
						*pOutput = 0xE0 + cZeroCounter;
						pOutput++;

						cZeroCounter = 0;
					}

					*pOutput = cOutChar;
					pOutput++;
				}

				cOutStage = 0;
			}
		}
		else
		{
			if (cZeroCounter != 0)
			{
				*pOutput = 0xE0 + cZeroCounter;
				pOutput++;

				cZeroCounter = 0;				
			}

			if (cOutStage == 0)
			{
				cOutChar = 0xFF;				
			}
			else
			{
				cOutChar += 0x0F;
				cOutStage = 0;
			}

			*pOutput = cOutChar;
			pOutput++;		

			if (nAbsDiffValue <= 63)
			{
				nDiffValue += 192;

				*pOutput = (XnUInt8)nDiffValue;
				pOutput++;
			}
			else
			{
				*(XnUInt16*)pOutput = (nCurrValue << 8) + (nCurrValue >> 8);
				pOutput+=2;
			}
		}

		nLastValue = nCurrValue;
		pInput++;
	}

	if (cOutStage != 0)
	{
		*pOutput = cOutChar + 0x0D;
		pOutput++;
	}

	if (cZeroCounter != 0)
	{
		*pOutput = 0xE0 + cZeroCounter;
		pOutput++;
	}

	*pnOutputSize = (XnUInt32)(pOutput - pOrigOutput);

	// All is good...
	return (XN_STATUS_OK);
}

XnStatus XnStreamCompressDepth16ZWithEmbTable(const XnUInt16* pInput, const XnUInt32 nInputSize, XnUInt8* pOutput, XnUInt32* pnOutputSize, XnUInt16 nMaxValue)
{
	// Local function variables
	const XnUInt16* pInputEnd = pInput + (nInputSize / sizeof(XnUInt16));
	const XnUInt16* pOrigInput = pInput;
	const XnUInt8* pOrigOutput = pOutput;
	XnUInt16 nCurrValue = 0;
	XnUInt16 nLastValue = 0;
	XnUInt16 nAbsDiffValue = 0;
	XnInt16 nDiffValue = 0;
	XnUInt8 cOutStage = 0;
	XnUInt8 cOutChar = 0;
	XnUInt8 cZeroCounter = 0;
	static XnUInt16 nEmbTable[XN_MAX_UINT16];
	XnUInt16 nEmbTableIdx=0;

	// Note: this function does not make sure it stay within the output memory boundaries!

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(pInput);
	XN_VALIDATE_INPUT_PTR(pOutput);
	XN_VALIDATE_INPUT_PTR(pnOutputSize);

	// Create the embedded value translation table...
	pOutput+=2;
	xnOSMemSet(&nEmbTable[0], 0, nMaxValue*sizeof(XnUInt16));

	while (pInput != pInputEnd)
	{
		nEmbTable[*pInput] = 1;
		pInput++;
	}

	for (XnUInt32 i=0; i<nMaxValue; i++)
	{
		if (nEmbTable[i] == 1)
		{
			nEmbTable[i] = nEmbTableIdx;
			nEmbTableIdx++;
			*(XnUInt16*)pOutput = XN_PREPARE_VAR16_IN_BUFFER(XnUInt16(i));
			pOutput+=2;
		}
	}

	*(XnUInt16*)(pOrigOutput) = XN_PREPARE_VAR16_IN_BUFFER(nEmbTableIdx);

	// Encode the data...
	pInput = pOrigInput;
	nLastValue = nEmbTable[*pInput];
	*(XnUInt16*)pOutput = XN_PREPARE_VAR16_IN_BUFFER(nLastValue);
	pInput++;
	pOutput+=2;

// 	for (XnUInt32 i = 0; i < nEmbTableIdx; i++)
// 		nEmbTable[i] = XN_PREPARE_VAR16_IN_BUFFER(nEmbTable[i]);


	while (pInput < pInputEnd)
	{	
		nCurrValue = nEmbTable[*pInput];

		nDiffValue = (nLastValue - nCurrValue);
		nAbsDiffValue = (XnUInt16)abs(nDiffValue);

		if (nAbsDiffValue <= 6)
		{
			nDiffValue += 6;

			if (cOutStage == 0)
			{
				cOutChar = (XnUInt8)(nDiffValue << 4);

				cOutStage = 1;
			}
			else
			{
				cOutChar += (XnUInt8)nDiffValue;

				if (cOutChar == 0x66)
				{
					cZeroCounter++;

					if (cZeroCounter == 15)
					{
						*pOutput = 0xEF;
						pOutput++;

						cZeroCounter = 0;
					}
				}
				else
				{
					if (cZeroCounter != 0)
					{
						*pOutput = 0xE0 + cZeroCounter;
						pOutput++;

						cZeroCounter = 0;
					}

					*pOutput = cOutChar;
					pOutput++;
				}

				cOutStage = 0;
			}
		}
		else
		{
			if (cZeroCounter != 0)
			{
				*pOutput = 0xE0 + cZeroCounter;
				pOutput++;

				cZeroCounter = 0;				
			}

			if (cOutStage == 0)
			{
				cOutChar = 0xFF;				
			}
			else
			{
				cOutChar += 0x0F;
				cOutStage = 0;
			}

			*pOutput = cOutChar;
			pOutput++;		

			if (nAbsDiffValue <= 63)
			{
				nDiffValue += 192;

				*pOutput = (XnUInt8)nDiffValue;
				pOutput++;
			}
			else
			{
				*(XnUInt16*)pOutput = XN_PREPARE_VAR16_IN_BUFFER((nCurrValue << 8) + (nCurrValue >> 8));
				pOutput+=2;
			}
		}

		nLastValue = nCurrValue;
		pInput++;
	}

	if (cOutStage != 0)
	{
		*pOutput = cOutChar + 0x0D;
		pOutput++;
	}

	if (cZeroCounter != 0)
	{
		*pOutput = 0xE0 + cZeroCounter;
		pOutput++;
	}

	*pnOutputSize = (XnUInt32)(pOutput - pOrigOutput);

	// All is good...
	return (XN_STATUS_OK);
}

XnStatus XnStreamUncompressDepth16Z(const XnUInt8* pInput, const XnUInt32 nInputSize, XnUInt16* pOutput, XnUInt32* pnOutputSize)
{
	// Local function variables
	const XnUInt8* pInputEnd = pInput + nInputSize;
	XnUInt16* pOutputEnd = 0;
	const XnUInt16* pOrigOutput = pOutput;
	XnUInt16 nLastFullValue = 0;
	XnUInt8 cInput = 0;
	XnUInt8 cZeroCounter = 0;
	XnInt8 cInData1 = 0;
	XnInt8 cInData2 = 0;
	XnUInt8 cInData3 = 0;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(pInput);
	XN_VALIDATE_INPUT_PTR(pOutput);
	XN_VALIDATE_INPUT_PTR(pnOutputSize);

	if (nInputSize < sizeof(XnUInt16))
	{
		xnLogError(XN_MASK_STREAM_COMPRESSION, "Input size too small");
		return (XN_STATUS_BAD_PARAM);
	}

	pOutputEnd = pOutput + (*pnOutputSize / sizeof(XnUInt16));

	// Decode the data...
	nLastFullValue = *(XnUInt16*)pInput;
	*pOutput = nLastFullValue;
	pInput+=2;
	pOutput++;

	while (pInput != pInputEnd)
	{
		cInput = *pInput;

		if (cInput < 0xE0)
		{		
			cInData1 = cInput >> 4;
			cInData2 = (cInput & 0x0f);		

			nLastFullValue -= (cInData1 - 6);
			XN_CHECK_OUTPUT_OVERFLOW(pOutput, pOutputEnd);
			*pOutput = nLastFullValue;
			pOutput++;

			if (cInData2 != 0x0f) 
			{
				if (cInData2 != 0x0d)
				{
					nLastFullValue -= (cInData2 - 6);
					XN_CHECK_OUTPUT_OVERFLOW(pOutput, pOutputEnd);
					*pOutput = nLastFullValue;
					pOutput++;
				}

				pInput++;
			}
			else
			{
				pInput++;

				cInData3 = *pInput;
				if (cInData3 & 0x80)
				{
					nLastFullValue -= (cInData3 - 192);

					XN_CHECK_OUTPUT_OVERFLOW(pOutput, pOutputEnd);
					*pOutput = nLastFullValue;

					pOutput++;
					pInput++;
				}
				else
				{
					nLastFullValue = cInData3 << 8;
					pInput++;
					nLastFullValue += *pInput;

					XN_CHECK_OUTPUT_OVERFLOW(pOutput, pOutputEnd);
					*pOutput = nLastFullValue;

					pOutput++;
					pInput++;
				}
			}
		}
		else if (cInput == 0xFF)
		{
			pInput++;

			cInData3 = *pInput;

			if (cInData3 & 0x80)
			{
				nLastFullValue -= (cInData3 - 192);

				XN_CHECK_OUTPUT_OVERFLOW(pOutput, pOutputEnd);
				*pOutput = nLastFullValue;

				pInput++;
				pOutput++;
			}
			else
			{
				nLastFullValue = cInData3 << 8;
				pInput++;
				nLastFullValue += *pInput;

				XN_CHECK_OUTPUT_OVERFLOW(pOutput, pOutputEnd);
				*pOutput = nLastFullValue;

				pInput++;
				pOutput++;
			}
		}
		else //It must be 0xE?
		{
			cZeroCounter = cInput - 0xE0;

			while (cZeroCounter != 0)
			{
				XN_CHECK_OUTPUT_OVERFLOW(pOutput+1, pOutputEnd);
				*pOutput = nLastFullValue;						
				pOutput++;

				*pOutput = nLastFullValue;						
				pOutput++;

				cZeroCounter--;
			}

			pInput++;
		}			
	}

	*pnOutputSize = (XnUInt32)((pOutput - pOrigOutput) * sizeof(XnUInt16));

	// All is good...
	return (XN_STATUS_OK);
}

XnStatus XnStreamUncompressDepth16ZWithEmbTable(const XnUInt8* pInput, const XnUInt32 nInputSize, XnUInt16* pOutput, XnUInt32* pnOutputSize)
{
	// Local function variables
	const XnUInt8* pInputEnd = pInput + nInputSize;
	XnUInt16* pOutputEnd = 0;
	XnUInt16* pOrigOutput = pOutput;
	XnUInt16 nLastFullValue = 0;
	XnUInt8 cInput = 0;
	XnUInt8 cZeroCounter = 0;
	XnInt8 cInData1 = 0;
	XnInt8 cInData2 = 0;
	XnUInt8 cInData3 = 0;
	XnUInt16* pEmbTable = NULL;
	XnUInt16 nEmbTableIdx = 0;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(pInput);
	XN_VALIDATE_INPUT_PTR(pOutput);
	XN_VALIDATE_INPUT_PTR(pnOutputSize);

	if (nInputSize < sizeof(XnUInt16))
	{
		xnLogError(XN_MASK_STREAM_COMPRESSION, "Input size too small");
		return (XN_STATUS_BAD_PARAM);
	}

	nEmbTableIdx = XN_PREPARE_VAR16_IN_BUFFER(*(XnUInt16*)pInput);
	pInput+=2;
	pEmbTable = (XnUInt16*)pInput;
	pInput+=nEmbTableIdx * 2;
	for (XnUInt32 i = 0; i < nEmbTableIdx; i++)
		pEmbTable[i] = XN_PREPARE_VAR16_IN_BUFFER(pEmbTable[i]);

	pOutputEnd = pOutput + (*pnOutputSize / sizeof(XnUInt16));

	// Decode the data...
	nLastFullValue = XN_PREPARE_VAR16_IN_BUFFER(*(XnUInt16*)pInput);
	*pOutput = pEmbTable[nLastFullValue];
	pInput+=2;
	pOutput++;

	while (pInput != pInputEnd)
	{
		cInput = *pInput;

		if (cInput < 0xE0)
		{		
			cInData1 = cInput >> 4;
			cInData2 = (cInput & 0x0f);		

			nLastFullValue -= (cInData1 - 6);
			XN_CHECK_OUTPUT_OVERFLOW(pOutput, pOutputEnd);
			*pOutput =  pEmbTable[nLastFullValue];
			pOutput++;

			if (cInData2 != 0x0f) 
			{
				if (cInData2 != 0x0d)
				{
					nLastFullValue -= (cInData2 - 6);
					XN_CHECK_OUTPUT_OVERFLOW(pOutput, pOutputEnd);
					*pOutput =  pEmbTable[nLastFullValue];
					pOutput++;
				}

				pInput++;
			}
			else
			{
				pInput++;

				cInData3 = *pInput;
				if (cInData3 & 0x80)
				{
					nLastFullValue -= (cInData3 - 192);

					XN_CHECK_OUTPUT_OVERFLOW(pOutput, pOutputEnd);
					*pOutput =  pEmbTable[nLastFullValue];

					pOutput++;
					pInput++;
				}
				else
				{
					nLastFullValue = cInData3 << 8;
					pInput++;
					nLastFullValue += *pInput;

					XN_CHECK_OUTPUT_OVERFLOW(pOutput, pOutputEnd);
					*pOutput =  pEmbTable[nLastFullValue];

					pOutput++;
					pInput++;
				}
			}
		}
		else if (cInput == 0xFF)
		{
			pInput++;

			cInData3 = *pInput;

			if (cInData3 & 0x80)
			{
				nLastFullValue -= (cInData3 - 192);

				XN_CHECK_OUTPUT_OVERFLOW(pOutput, pOutputEnd);
				*pOutput =  pEmbTable[nLastFullValue];

				pInput++;
				pOutput++;
			}
			else
			{
				nLastFullValue = cInData3 << 8;
				pInput++;
				nLastFullValue += *pInput;

				XN_CHECK_OUTPUT_OVERFLOW(pOutput, pOutputEnd);
				*pOutput =  pEmbTable[nLastFullValue];

				pInput++;
				pOutput++;
			}
		}
		else //It must be 0xE?
		{
			cZeroCounter = cInput - 0xE0;

			while (cZeroCounter != 0)
			{
				XN_CHECK_OUTPUT_OVERFLOW(pOutput+1, pOutputEnd);
				*pOutput =  pEmbTable[nLastFullValue];
				pOutput++;

				*pOutput =  pEmbTable[nLastFullValue];
				pOutput++;

				cZeroCounter--;
			}

			pInput++;
		}			
	}

	*pnOutputSize = (XnUInt32)((pOutput - pOrigOutput) * sizeof(XnUInt16));

	// All is good...
	return (XN_STATUS_OK);
}

XnStatus XnStreamCompressImage8Z(const XnUInt8* pInput, const XnUInt32 nInputSize, XnUInt8* pOutput, XnUInt32* pnOutputSize)
{
	// Local function variables
	const XnUInt8* pInputEnd = pInput + nInputSize;
	const XnUInt8* pOrigOutput = pOutput;
	XnUInt8 nCurrValue = 0;
	XnUInt8 nLastValue = 0;
	XnUInt8 nAbsDiffValue = 0;
	XnInt8 nDiffValue = 0;
	XnUInt8 cOutStage = 0;
	XnUInt8 cOutChar = 0;
	XnUInt8 cZeroCounter = 0;
	XnBool bFlag = FALSE;

	// Note: this function does not make sure it stay within the output memory boundaries!

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(pInput);
	XN_VALIDATE_INPUT_PTR(pOutput);
	XN_VALIDATE_INPUT_PTR(pnOutputSize);

	// Encode the data...
	nLastValue = *pInput;
	*pOutput = nLastValue;
	pInput++;
	pOutput++;

	while (pInput != pInputEnd)
	{	
		nCurrValue = *pInput;

		nDiffValue = (nLastValue - nCurrValue);
		nAbsDiffValue = (XnUInt8)abs(nDiffValue);

		if (nAbsDiffValue <= 6)
		{
			nDiffValue += 6;

			if (cOutStage == 0)
			{
				cOutChar = nDiffValue << 4;

				cOutStage = 1;
			}
			else
			{
				cOutChar += nDiffValue;

				if ((cOutChar == 0x66) && (bFlag == FALSE))
				{
					cZeroCounter++;

					if (cZeroCounter == 15)
					{
						*pOutput = 0xEF;
						pOutput++;

						cZeroCounter = 0;
					}
				}
				else
				{
					if (cZeroCounter != 0)
					{
						*pOutput = 0xE0 + cZeroCounter;
						pOutput++;

						cZeroCounter = 0;
					}

					*pOutput = cOutChar;
					pOutput++;

					bFlag = FALSE;
				}

				cOutStage = 0;
			}
		}
		else
		{
			if (cZeroCounter != 0)
			{
				*pOutput = 0xE0 + cZeroCounter;
				pOutput++;

				cZeroCounter = 0;				
			}

			if (cOutStage == 0)
			{
				cOutChar = 0xF0;		
				cOutChar += nCurrValue >> 4;

				*pOutput = cOutChar;
				pOutput++;		

				cOutChar = (nCurrValue & 0xF) << 4;
				cOutStage = 1;

				bFlag = TRUE;
			}
			else
			{
				cOutChar += 0x0F;
				cOutStage = 0;

				*pOutput = cOutChar;
				pOutput++;		

				*pOutput = nCurrValue;
				pOutput++;
			}
		}

		nLastValue = nCurrValue;
		pInput++;
	}

	if (cOutStage != 0)
	{
		*pOutput = cOutChar + 0x0D;
		pOutput++;
	}

	if (cZeroCounter != 0)
	{
		*pOutput = 0xE0 + cZeroCounter;
		pOutput++;
	}

	*pnOutputSize = (XnUInt32)(pOutput - pOrigOutput);

	// All is good...
	return (XN_STATUS_OK);
}

XnStatus XnStreamUncompressImage8Z(const XnUInt8* pInput, const XnUInt32 nInputSize, XnUInt8* pOutput, XnUInt32* pnOutputSize)
{
	const XnUInt8* pInputEnd = pInput + nInputSize;
	const XnUInt8* pOrigOutput = pOutput;
	XnUInt8 nLastFullValue = 0;
	XnUInt8 cInput = 0;
	XnUInt8 cZeroCounter = 0;
	XnInt8 cInData1 = 0;
	XnInt8 cInData2 = 0;

	// Note: this function does not make sure it stay within the output memory boundaries!

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(pInput);
	XN_VALIDATE_INPUT_PTR(pOutput);
	XN_VALIDATE_INPUT_PTR(pnOutputSize);

	if (nInputSize < sizeof(XnUInt8))
	{
		xnLogError(XN_MASK_STREAM_COMPRESSION, "Input size too small");
		return (XN_STATUS_BAD_PARAM);
	}

	// Decode the data...
	nLastFullValue = *pInput;
	*pOutput = nLastFullValue;
	pInput++;
	pOutput++;

	while (pInput != pInputEnd)
	{
		cInput = *pInput;

		if (cInput < 0xE0)
		{		
			cInData1 = cInput >> 4;
			cInData2 = (cInput & 0x0f);		

			nLastFullValue -= (cInData1 - 6);
			*pOutput = nLastFullValue;
			pOutput++;

			if (cInData2 != 0x0f) 
			{
				if (cInData2 != 0x0d)
				{
					nLastFullValue -= (cInData2 - 6);
					*pOutput = nLastFullValue;
					pOutput++;
				}
			}
			else
			{
				pInput++;
				nLastFullValue = *pInput;
				*pOutput = nLastFullValue;
				pOutput++;
			}

			pInput++;
		}
		else if (cInput >= 0xF0)
		{
			cInData1 = cInput << 4;		

			pInput++;
			cInput = *pInput;

			nLastFullValue = cInData1 + (cInput >> 4);	

			*pOutput = nLastFullValue;
			pOutput++;

			cInData2 = cInput & 0xF;

			if (cInData2 == 0x0F) 
			{
				pInput++;
				nLastFullValue = *pInput;
				*pOutput = nLastFullValue;
				pOutput++;
				pInput++;
			}
			else
			{
				if (cInData2 != 0x0D)
				{
					nLastFullValue -= (cInData2 - 6);
					*pOutput = nLastFullValue;
					pOutput++;
				}

				pInput++;
			}
		}
		else //It must be 0xE?
		{
			cZeroCounter = cInput - 0xE0;

			while (cZeroCounter != 0)
			{
				*pOutput = nLastFullValue;						
				pOutput++;

				*pOutput = nLastFullValue;						
				pOutput++;

				cZeroCounter--;
			}

			pInput++;
		}			
	}

	*pnOutputSize = (XnUInt32)(pOutput - pOrigOutput);

	// All is good...
	return (XN_STATUS_OK);
}

XnStatus XnStreamCompressConf4(const XnUInt8* pInput, const XnUInt32 nInputSize, XnUInt8* pOutput, XnUInt32* pnOutputSize)
{
	// Local function variables
	const XnUInt8* pInputEnd = pInput + nInputSize;
	const XnUInt8* pOrigOutput = pOutput;

	// Note: this function does not make sure it stay within the output memory boundaries!

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(pInput);
	XN_VALIDATE_INPUT_PTR(pOutput);
	XN_VALIDATE_INPUT_PTR(pnOutputSize);

	// Encode the data...
	while (pInput != pInputEnd)
	{
		*pOutput = *pInput << 4;
		pInput++;
		
		*pOutput += *pInput;
		pInput++;

		pOutput++;
	}

	*pnOutputSize = (XnUInt32)(pOutput - pOrigOutput);

	// All is good...
	return (XN_STATUS_OK);
}

XnStatus XnStreamUncompressConf4(const XnUInt8* pInput, const XnUInt32 nInputSize, XnUInt8* pOutput, XnUInt32* pnOutputSize)
{
	// Local function variables
	const XnUInt8* pInputEnd = pInput + nInputSize;
	const XnUInt8* pOutputEnd = 0;
	const XnUInt8* pOrigOutput = pOutput;
	XnUInt8 nValue1;
	XnUInt8 nValue2;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(pInput);
	XN_VALIDATE_INPUT_PTR(pOutput);
	XN_VALIDATE_INPUT_PTR(pnOutputSize);

	if (nInputSize < sizeof(XnUInt8))
	{
		xnLogError(XN_MASK_STREAM_COMPRESSION, "Input size too small");
		return (XN_STATUS_BAD_PARAM);
	}

	if (nInputSize % 2 != 0)
	{
		xnLogError(XN_MASK_STREAM_COMPRESSION, "Input size not word-aligned");
		return (XN_STATUS_BAD_PARAM);
	}

	pOutputEnd = pOutput + *pnOutputSize;

	XN_CHECK_OUTPUT_OVERFLOW(pOutput + (nInputSize * 2), pOutputEnd);

	while (pInput != pInputEnd)
	{
		nValue1 = pInput[0];
		nValue2 = pInput[1];

		pOutput[0] = nValue1 >> 4;
		pOutput[1] = nValue1 & 0xF;
		pOutput[2] = nValue2 >> 4;
		pOutput[3] = nValue2 & 0xF;

		pOutput+=4;
		pInput+=2;
	}

	*pnOutputSize = (XnUInt32)(pOutput - pOrigOutput);

	// All is good...
	return (XN_STATUS_OK);
}

void XnStreamJPEGCompDummyFunction(struct jpeg_compress_struct* /*pjCompStruct*/)
{
	// Dummy libjpeg function to wrap internal buffers usage...
}

boolean XnStreamJPEGCompDummyFailFunction(struct jpeg_compress_struct* /*pjCompStruct*/)
{
	// If we ever got to the point we need to allocate more memory, something is wrong!
	return (FALSE);
}

void XnStreamJPEGDummyErrorExit(j_common_ptr cinfo)
{
	XnLibJpegErrorMgr* errMgr = (XnLibJpegErrorMgr*)cinfo->err; 

	longjmp(errMgr->setjmpBuffer, 1); 
}

void XnStreamJPEGOutputMessage(j_common_ptr cinfo)
{
	struct jpeg_error_mgr* err = cinfo->err;
	int msg_code = err->msg_code;
	if (msg_code == JWRN_EXTRANEOUS_DATA)
	{
		// NOTE: we are aware this problem occurs. Log a warning every once in a while
		static XnUInt32 nTimes = 0;
		if (++nTimes == 50)
		{
			char buffer[JMSG_LENGTH_MAX];

			/* Create the message */
			(*cinfo->err->format_message) (cinfo, buffer);

			//Temporary disabled this error since it happens all the time and it's a known issue.
			//xnLogWarning(XN_MASK_JPEG, "JPEG: The following warning occurred 50 times: %s", buffer);
			nTimes = 0;
		}
	}
	else
	{
		char buffer[JMSG_LENGTH_MAX];

		/* Create the message */
		(*cinfo->err->format_message) (cinfo, buffer);

		xnLogWarning(XN_MASK_JPEG, "JPEG: %s", buffer);
	}
}

XnStatus XnStreamInitCompressImageJ(XnStreamCompJPEGContext* pStreamCompJPEGContext)
{
	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_OUTPUT_PTR(pStreamCompJPEGContext);

	pStreamCompJPEGContext->jCompStruct.err = jpeg_std_error(&pStreamCompJPEGContext->jErrMgr.pub);
	pStreamCompJPEGContext->jErrMgr.pub.output_message = XnStreamJPEGOutputMessage;
	pStreamCompJPEGContext->jErrMgr.pub.error_exit = XnStreamJPEGDummyErrorExit;

	jpeg_create_compress(&pStreamCompJPEGContext->jCompStruct);

	pStreamCompJPEGContext->jCompStruct.dest = &pStreamCompJPEGContext->jDestMgr;
	pStreamCompJPEGContext->jCompStruct.dest->init_destination = XnStreamJPEGCompDummyFunction;
	pStreamCompJPEGContext->jCompStruct.dest->empty_output_buffer = XnStreamJPEGCompDummyFailFunction;
	pStreamCompJPEGContext->jCompStruct.dest->term_destination = XnStreamJPEGCompDummyFunction;

	// All is good...
	return (XN_STATUS_OK);
}

XnStatus XnStreamFreeCompressImageJ(XnStreamCompJPEGContext* pStreamCompJPEGContext)
{
	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(pStreamCompJPEGContext);

	jpeg_destroy_compress(&pStreamCompJPEGContext->jCompStruct);

	// All is good...
	return (XN_STATUS_OK);
}

// to allow the use of setjmp
#pragma warning(push)
#pragma warning(disable: 4611)

XnStatus XnStreamCompressImage8J(XnStreamCompJPEGContext* pStreamCompJPEGContext, const XnUInt8* pInput, XnUInt8* pOutput, XnUInt32* pnOutputSize, const XnUInt32 nXRes, const XnUInt32 nYRes, const XnUInt32 nQuality)
{
	// Local function variables
	XnUInt8* pCurrScanline = (XnUInt8*)pInput;
	XnUInt32 nYIndex = 0;
	jpeg_compress_struct* pjCompStruct = NULL;	

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(pStreamCompJPEGContext);
	XN_VALIDATE_INPUT_PTR(pInput);
	XN_VALIDATE_OUTPUT_PTR(pOutput);
	XN_VALIDATE_OUTPUT_PTR(pnOutputSize);

	pjCompStruct = &pStreamCompJPEGContext->jCompStruct;

	if (setjmp(pStreamCompJPEGContext->jErrMgr.setjmpBuffer))
	{
		//If we get here, the JPEG code has signaled an error.
		XnStreamFreeCompressImageJ(pStreamCompJPEGContext);
		XnStreamInitCompressImageJ(pStreamCompJPEGContext);

		*pnOutputSize = 0;

		xnLogError(XN_MASK_JPEG, "JPEG compressor error :(");
		return (XN_STATUS_ERROR);
	} 

	pjCompStruct->in_color_space = JCS_GRAYSCALE;
	jpeg_set_defaults(pjCompStruct);
	pjCompStruct->input_components = 1;
	pjCompStruct->num_components = 1;
	pjCompStruct->image_width = nXRes;
	pjCompStruct->image_height = nYRes;
	pjCompStruct->data_precision = 8;
	pjCompStruct->input_gamma = 1.0;

	jpeg_set_quality(pjCompStruct, nQuality, FALSE);

	pjCompStruct->dest->next_output_byte = (JOCTET*)pOutput;
	pjCompStruct->dest->free_in_buffer = *pnOutputSize;

	jpeg_start_compress(pjCompStruct, TRUE);

	for (nYIndex = 0; nYIndex < nYRes; nYIndex++)
	{
		jpeg_write_scanlines(pjCompStruct, &pCurrScanline, 1);

		pCurrScanline += nXRes;
	}

	jpeg_finish_compress(pjCompStruct);

	*pnOutputSize -= (XnUInt32)pjCompStruct->dest->free_in_buffer;

	// All is good...
	return (XN_STATUS_OK);
}

XnStatus XnStreamCompressImage24J(XnStreamCompJPEGContext* pStreamCompJPEGContext, const XnUInt8* pInput, XnUInt8* pOutput, XnUInt32* pnOutputSize, const XnUInt32 nXRes, const XnUInt32 nYRes, const XnUInt32 nQuality)
{
	// Local function variables
	XnUInt8* pCurrScanline = (XnUChar*)pInput;
	XnUInt32 nYIndex = 0;
	XnUInt32 nScanLineSize = 0;
	jpeg_compress_struct* pjCompStruct = NULL;	

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(pStreamCompJPEGContext);
	XN_VALIDATE_INPUT_PTR(pInput);
	XN_VALIDATE_OUTPUT_PTR(pOutput);
	XN_VALIDATE_OUTPUT_PTR(pnOutputSize);

	pjCompStruct = &pStreamCompJPEGContext->jCompStruct;

	if (setjmp(pStreamCompJPEGContext->jErrMgr.setjmpBuffer))
	{
		//If we get here, the JPEG code has signaled an error.
		XnStreamFreeCompressImageJ(pStreamCompJPEGContext);
		XnStreamInitCompressImageJ(pStreamCompJPEGContext);

		*pnOutputSize = 0;

		xnLogError(XN_MASK_JPEG, "JPEG compressor error :(");
		return (XN_STATUS_ERROR);
	} 

	pjCompStruct->in_color_space = JCS_RGB;
	jpeg_set_defaults(pjCompStruct);
	pjCompStruct->input_components = 3;
	pjCompStruct->num_components = 3;
	pjCompStruct->image_width = nXRes;
	pjCompStruct->image_height = nYRes;
	pjCompStruct->data_precision = 8;
	pjCompStruct->input_gamma = 1.0;

	jpeg_set_quality(pjCompStruct, nQuality, FALSE);

	pjCompStruct->dest->next_output_byte = (JOCTET*)pOutput;
	pjCompStruct->dest->free_in_buffer = *pnOutputSize;

	jpeg_start_compress(pjCompStruct, TRUE);

	nScanLineSize = nXRes * 3;
	for (nYIndex = 0; nYIndex < nYRes; nYIndex++)
	{
		jpeg_write_scanlines(pjCompStruct, &pCurrScanline, 1);

		pCurrScanline += nScanLineSize;
	}

	jpeg_finish_compress(pjCompStruct);

	*pnOutputSize -= (XnUInt32)pjCompStruct->dest->free_in_buffer;

	// All is good...
	return (XN_STATUS_OK);
}

void XnStreamJPEGDecompDummyFunction(struct jpeg_decompress_struct* /*pjDecompStruct*/)
{
	// Dummy libjpeg function to wrap internal buffers usage...
}

boolean XnStreamJPEGDecompDummyFailFunction(struct jpeg_decompress_struct* /*pjDecompStruct*/)
{
	// If we ever got to the point we need to allocate more memory, something is wrong!
	return (FALSE);
}

void XnStreamJPEGDecompSkipFunction(struct jpeg_decompress_struct* pjDecompStruct, long nNumBytes)
{
	// Skip bytes in the internal buffer
	pjDecompStruct->src->next_input_byte += (size_t)nNumBytes;
	pjDecompStruct->src->bytes_in_buffer -= (size_t)nNumBytes;
}

XnStatus XnStreamInitUncompressImageJ(XnStreamUncompJPEGContext* pStreamUncompJPEGContext)
{
	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_OUTPUT_PTR(pStreamUncompJPEGContext);

	pStreamUncompJPEGContext->jDecompStruct.err = jpeg_std_error(&pStreamUncompJPEGContext->jErrMgr.pub);
	pStreamUncompJPEGContext->jErrMgr.pub.output_message = XnStreamJPEGOutputMessage;
	pStreamUncompJPEGContext->jErrMgr.pub.error_exit = XnStreamJPEGDummyErrorExit;

	jpeg_create_decompress(&pStreamUncompJPEGContext->jDecompStruct);

	pStreamUncompJPEGContext->jDecompStruct.src = &pStreamUncompJPEGContext->jSrcMgr;
	pStreamUncompJPEGContext->jDecompStruct.src->init_source = XnStreamJPEGDecompDummyFunction;
	pStreamUncompJPEGContext->jDecompStruct.src->fill_input_buffer = XnStreamJPEGDecompDummyFailFunction;
	pStreamUncompJPEGContext->jDecompStruct.src->skip_input_data = XnStreamJPEGDecompSkipFunction;
	pStreamUncompJPEGContext->jDecompStruct.src->resync_to_restart = jpeg_resync_to_restart;
	pStreamUncompJPEGContext->jDecompStruct.src->term_source = XnStreamJPEGDecompDummyFunction;

	// All is good...
	return (XN_STATUS_OK);
}

XnStatus XnStreamFreeUncompressImageJ(XnStreamUncompJPEGContext* pStreamUncompJPEGContext)
{
	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(pStreamUncompJPEGContext);

	jpeg_destroy_decompress(&pStreamUncompJPEGContext->jDecompStruct);

	// All is good...
	return (XN_STATUS_OK);
}

XnStatus XnStreamUncompressImageJ(XnStreamUncompJPEGContext* pStreamUncompJPEGContext, const XnUInt8* pInput, const XnUInt32 nInputSize, XnUInt8* pOutput, XnUInt32* pnOutputSize)
{
	// Local function variables
	XnUInt8* pCurrScanline = pOutput;
	XnUInt8* pNextScanline = NULL;
	XnUInt8* pOutputEnd = 0;
	XnUInt32 nScanLineSize = 0;
	XnUInt32 nOutputSize = 0;
	jpeg_decompress_struct* pjDecompStruct = NULL;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(pStreamUncompJPEGContext);
	XN_VALIDATE_INPUT_PTR(pInput);
	XN_VALIDATE_OUTPUT_PTR(pOutput);
	XN_VALIDATE_OUTPUT_PTR(pnOutputSize);

	if (nInputSize == 0)
	{
		xnLogError(XN_MASK_JPEG, "Input size is 0");
		return (XN_STATUS_BAD_PARAM);
	}

	pOutputEnd = pOutput + *pnOutputSize;

	pjDecompStruct = &pStreamUncompJPEGContext->jDecompStruct;

	pjDecompStruct->src->bytes_in_buffer = nInputSize;
	pjDecompStruct->src->next_input_byte = pInput;

	if (setjmp(pStreamUncompJPEGContext->jErrMgr.setjmpBuffer))
	{
		//If we get here, the JPEG code has signaled an error.
		XnStreamFreeUncompressImageJ(pStreamUncompJPEGContext);
		XnStreamInitUncompressImageJ(pStreamUncompJPEGContext);

		*pnOutputSize = 0;

		xnLogError(XN_MASK_JPEG, "JPEG compressor error :(");
		return (XN_STATUS_ERROR);
	} 

	jpeg_read_header(pjDecompStruct, TRUE);

	jpeg_start_decompress(pjDecompStruct);

	nScanLineSize = pjDecompStruct->output_width * pjDecompStruct->num_components;

	nOutputSize = pjDecompStruct->output_height * nScanLineSize;
	if (nOutputSize > *pnOutputSize)
	{
		XnStreamFreeUncompressImageJ(pStreamUncompJPEGContext);
		XnStreamInitUncompressImageJ(pStreamUncompJPEGContext);

		*pnOutputSize = 0;

		xnLogError(XN_MASK_JPEG, "JPEG compressor error :(");
		return (XN_STATUS_OUTPUT_BUFFER_OVERFLOW);
	}

	while (pStreamUncompJPEGContext->jDecompStruct.output_scanline < pStreamUncompJPEGContext->jDecompStruct.output_height)
	{
		pNextScanline = pCurrScanline+nScanLineSize;

		if (pNextScanline > pOutputEnd)
		{
			XnStreamFreeUncompressImageJ(pStreamUncompJPEGContext);
			XnStreamInitUncompressImageJ(pStreamUncompJPEGContext);

			*pnOutputSize = 0;

			xnLogError(XN_MASK_JPEG, "JPEG compressor error :(");
			return (XN_STATUS_OUTPUT_BUFFER_OVERFLOW);
		}

		jpeg_read_scanlines(pjDecompStruct, &pCurrScanline, 1);
		pCurrScanline = pNextScanline;
	}

	jpeg_finish_decompress(pjDecompStruct);

	*pnOutputSize = nOutputSize;

	// All is good...
	return (XN_STATUS_OK);
}

#pragma warning(pop)
