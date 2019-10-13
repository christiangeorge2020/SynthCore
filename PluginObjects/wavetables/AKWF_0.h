#ifndef _akwf_0
#define _akwf_0

#include "testtable_0.h"
#include "testtable_1.h"
#include "testtable_2.h"
#include "testtable_3.h"
#include "testtable_4.h"
#include "testtable_5.h"
#include "testtable_6.h"
#include "testtable_7.h"
#include "testtable_8.h"
#include "testtable_9.h"
#include "testtable_10.h"
#include "testtable_11.h"
#include "testtable_12.h"
#include "testtable_13.h"
#include "testtable_14.h"
#include "testtable_15.h"
#include "testtable_16.h"
#include "testtable_17.h"
#include "testtable_18.h"
#include "testtable_19.h"
#include "testtable_20.h"
#include "testtable_21.h"
#include "testtable_22.h"
#include "testtable_23.h"
#include "testtable_24.h"
#include "testtable_25.h"
#include "testtable_26.h"
#include "testtable_27.h"
#include "testtable_28.h"
#include "testtable_29.h"
#include "testtable_30.h"
#include "testtable_31.h"

const unsigned int AKWF_0_TablePtrsCount = 32;

static HiResWTSet* AKWF_0_TablePtrs[AKWF_0_TablePtrsCount] = { &testtable_0_TableSet, &testtable_1_TableSet, &testtable_2_TableSet, &testtable_3_TableSet, &testtable_4_TableSet, &testtable_5_TableSet, &testtable_6_TableSet, &testtable_7_TableSet, &testtable_8_TableSet, &testtable_9_TableSet, &testtable_10_TableSet, &testtable_11_TableSet, &testtable_12_TableSet, &testtable_13_TableSet, &testtable_14_TableSet, &testtable_15_TableSet, &testtable_16_TableSet, &testtable_17_TableSet, &testtable_18_TableSet, &testtable_19_TableSet, &testtable_20_TableSet, &testtable_21_TableSet, &testtable_22_TableSet, &testtable_23_TableSet, &testtable_24_TableSet, &testtable_25_TableSet, &testtable_26_TableSet, &testtable_27_TableSet, &testtable_28_TableSet, &testtable_29_TableSet, &testtable_30_TableSet, &testtable_31_TableSet }; 

// --- Define Wavform Names: Here is where you can override the table names all at once;
//     Remember to keep the names short and simple, refrain from the underscore, and keep the character count below 32 for compatibiltiy with brick files.
static std::string AKWF_0_TableNames[AKWF_0_TablePtrsCount] = { 
	"testtable 0",
	"testtable 1",
	"testtable 2",
	"testtable 3",
	"testtable 4",
	"testtable 5",
	"testtable 6",
	"testtable 7",
	"testtable 8",
	"testtable 9",
	"testtable 10",
	"testtable 11",
	"testtable 12",
	"testtable 13",
	"testtable 14",
	"testtable 15",
	"testtable 16",
	"testtable 17",
	"testtable 18",
	"testtable 19",
	"testtable 20",
	"testtable 21",
	"testtable 22",
	"testtable 23",
	"testtable 24",
	"testtable 25",
	"testtable 26",
	"testtable 27",
	"testtable 28",
	"testtable 29",
	"testtable 30",
	"testtable 31" }; 

// --- This is the bank descriptor; you can initialize the bank with it. 
static BankDescriptor AKWF_0_BankDescriptor(AKWF_0_TablePtrsCount, &AKWF_0_TablePtrs[0], &AKWF_0_TableNames[0]);

#endif // definer
