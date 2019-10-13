#ifndef _akwf_1
#define _akwf_1

#include "willcode_0.h"
#include "willcode_1.h"
#include "willcode_2.h"
#include "willcode_3.h"
#include "willcode_4.h"
#include "willcode_5.h"
#include "willcode_6.h"
#include "willcode_7.h"
#include "willcode_8.h"
#include "willcode_9.h"
#include "willcode_10.h"
#include "willcode_11.h"
#include "willcode_12.h"
#include "willcode_13.h"
#include "willcode_14.h"
#include "willcode_15.h"
#include "willcode_16.h"
#include "willcode_17.h"
#include "willcode_18.h"
#include "willcode_19.h"

const unsigned int AKWF_1_TablePtrsCount = 20;

static HiResWTSet* AKWF_1_TablePtrs[AKWF_1_TablePtrsCount] = { &willcode_0_TableSet, &willcode_1_TableSet, &willcode_2_TableSet, &willcode_3_TableSet, &willcode_4_TableSet, &willcode_5_TableSet, &willcode_6_TableSet, &willcode_7_TableSet, &willcode_8_TableSet, &willcode_9_TableSet, &willcode_10_TableSet, &willcode_11_TableSet, &willcode_12_TableSet, &willcode_13_TableSet, &willcode_14_TableSet, &willcode_15_TableSet, &willcode_16_TableSet, &willcode_17_TableSet, &willcode_18_TableSet, &willcode_19_TableSet }; 

// --- Define Wavform Names: Here is where you can override the table names all at once;
//     Remember to keep the names short and simple, refrain from the underscore, and keep the character count below 32 for compatibiltiy with brick files.
static std::string AKWF_1_TableNames[AKWF_1_TablePtrsCount] = { 
	"willcode 0",
	"willcode 1",
	"willcode 2",
	"willcode 3",
	"willcode 4",
	"willcode 5",
	"willcode 6",
	"willcode 7",
	"willcode 8",
	"willcode 9",
	"willcode 10",
	"willcode 11",
	"willcode 12",
	"willcode 13",
	"willcode 14",
	"willcode 15",
	"willcode 16",
	"willcode 17",
	"willcode 18",
	"willcode 19" }; 

// --- This is the bank descriptor; you can initialize the bank with it. 
static BankDescriptor AKWF_1_BankDescriptor(AKWF_1_TablePtrsCount, &AKWF_1_TablePtrs[0], &AKWF_1_TableNames[0]);

#endif // definer
