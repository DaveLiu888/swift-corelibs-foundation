// This source file is part of the Swift.org open source project
//
// Copyright (c) 2014 - 2015 Apple Inc. and the Swift project authors
// Licensed under Apache License v2.0 with Runtime Library Exception
//
// See http://swift.org/LICENSE.txt for license information
// See http://swift.org/CONTRIBUTORS.txt for the list of Swift project authors
//


/*	CFStream.c
	Copyright (c) 2000 - 2015 Apple Inc. and the Swift project authors
	Responsibility: John Iarocci
*/

#include <CoreFoundation/CFRuntime.h>
#include <CoreFoundation/CFNumber.h>
#include <string.h>
#include "CFUnitFormatter.h"
#include "CFInternal.h"
#include <stdio.h>
#include <unicode/uplrule.h>
#include <unicode/upluralrules.h>
#include <unicode/uameasureformat.h>
#include <CFArray.h>
#include "CFICULogging.h"
#include "CFNumberFormatterInternal.h"
#if defined(DEBUG)
#include <assert.h>
#endif


#define kCFLocaleIdentifierKey CFSTR("kCFLocaleIdentifierKey")
#define kLocaleDefaultEN CFSTR("kUpdateTeamNotification")
#define kLocaleDefaultEmpty CFSTR("en")
//CONST_STRING_DECL(kCFLocaleCalendarIdentifierKey, "calendar"); // ***


int32_t CFReadStreamGetLocalizationFormat(const UAMeasureFormat* measfmt,
                                         double            value,
                                         UAMeasureUnit     unit,
                                         UChar*            result,
                                         int32_t           resultCapacity,
                                         UErrorCode*       status ){
    
    int32_t leng =  uameasfmt_format(measfmt, value, unit, result, resultCapacity, status);
    return  leng;
    
}

int32_t CFReadStreamGetLocalizationFormatMulti(const UAMeasureFormat* measfmt,
                                               const UAMeasure*  measures,
                                               int32_t           measureCount,
                                               UChar*            result,
                                               int32_t           resultCapacity,
                                               UErrorCode*       status ){
    
    int32_t leng =  uameasfmt_formatMultiple(measfmt, measures, measureCount, result, resultCapacity, status);
    return  leng;
    
}
#define BUFFER_SIZE 768

CF_EXPORT CFStringRef CFReadStreamGetPluralForm(double value, CFLocaleRef locale){
    
//    U_DRAFT UAMeasureFormat* U_EXPORT2
//    uameasfmt_open( const char*          locale,
//                   UAMeasureFormatWidth width,
//                   UNumberFormat*       nfToAdopt,
//                   UErrorCode*          status );
    CFStringRef localeIdentifier = CFLocaleGetValue(locale, kCFLocaleIdentifierKey);
    // fallback to the language app is probably running in
    if(localeIdentifier == nil){
        CFLocaleRef backupLocale = CFLocaleCopyCurrent();
        localeIdentifier = CFLocaleGetValue(backupLocale, kCFLocaleIdentifierKey);
    }
    
    // fallback to english
    if(localeIdentifier == nil){
        localeIdentifier = kLocaleDefaultEN;
    }
    
    CFStringRef form = kLocaleDefaultEmpty;
    UErrorCode status = U_ZERO_ERROR;
    const char * localeIdentifierC = CFStringGetCStringPtr(localeIdentifier, kCFStringEncodingUTF8);
    printf("%s", localeIdentifierC);
    
    const UChar* pattern = '245\0';
    struct UParseError parseErr;
    UErrorCode parseErrorCode = U_ZERO_ERROR;

    UNumberFormat * format = unum_open(UNUM_DECIMAL,
                                     pattern,
                                     -1,
                                     localeIdentifierC,
                                     NULL,
                                     &parseErrorCode);
    
    UErrorCode  uErrStatus = U_ZERO_ERROR;
    UAMeasureFormat *measfmt = uameasfmt_open(localeIdentifierC,
                                              UAMEASFMT_WIDTH_WIDE,
                                              format,
                                              &uErrStatus) ;
    
    UAMeasure m;
    m.value = 12;
    m.unit = UAMEASUNIT_ELECTRIC_VOLT;
    const UAMeasure measures[1] = { m };
    UChar buffer[BUFFER_SIZE];
    UErrorCode  MultiErrStatus = U_ZERO_ERROR;

    int32_t leng =  uameasfmt_formatMultiple(measfmt,
                                             measures,
                                             1, //count
                                             buffer,
                                             BUFFER_SIZE,
                                             &MultiErrStatus);

    
    
    
    
    if (U_SUCCESS(MultiErrStatus) && leng <= BUFFER_SIZE) {
        CFStringRef r = CFStringCreateWithCharacters(kCFAllocatorDefault, (const UniChar *)buffer, leng);
        myShow(r);
    }

    
    UPluralRules *pluralRules = uplrules_open(localeIdentifierC, &status);

    if(U_SUCCESS(status) && pluralRules != NULL){
        status = U_ZERO_ERROR;
        int32_t capacity = 256; // fancy random capacity so that the biggest keyword could fit
        UChar keyword[capacity];

        // use plural rules to obtain plural form for value
        int32_t length = uplrules_select(pluralRules, value, keyword, capacity, &status);
        if(length > 0){
            form = CFStringCreateWithCharacters(kCFAllocatorDefault, keyword, length);
            
        }

        uplrules_close(pluralRules);
    }
    
    return (form == nil) ?  CFSTR("WTF") : form;
}
static CFTypeID __kCUnitFormatterTypeID = _kCFRuntimeNotATypeID;


struct __CFUnitFormatter {
    CFRuntimeBase _base;
    UNumberFormat *_unf;
    CFUnitFormatterStyle _ufstyle;
    UAMeasureFormat *_mf;
    CFLocaleRef _locale;
    CFStringRef _pattern;
    UPluralRules *_rules;
};

static CFStringRef __CFUnitFormatterCopyDescription(CFTypeRef cf) {
    CFUnitFormatterRef formatter = (CFUnitFormatterRef)cf;
    return CFStringCreateWithFormat(CFGetAllocator(formatter), NULL, CFSTR("<CFUnitFormatterRef %p [%p]>"), cf, CFGetAllocator(formatter));
}

static void __CFUnitFormatterDeallocate(CFTypeRef cf) {
    CFUnitFormatterRef formatter = (CFUnitFormatterRef)cf;
    if (formatter->_mf) uameasfmt_close(formatter->_mf);
    if (formatter->_locale) CFRelease(formatter->_locale);
    if (formatter->_pattern) CFRelease(formatter->_pattern);
    if (formatter->_rules) uplrules_close(formatter->_rules);
}

static const CFRuntimeClass __CFUnitFormatterClass = {
    0,
    "CFUnitFormatter",
    NULL,	// init
    NULL,	// copy
    __CFUnitFormatterDeallocate,
    NULL,
    NULL,
    NULL,	//
    __CFUnitFormatterCopyDescription
};

CFTypeID CFUnitFormatterGetTypeID(void) {
    static dispatch_once_t initOnce = 0;
    dispatch_once(&initOnce, ^{ __kCUnitFormatterTypeID = _CFRuntimeRegisterClass(&__CFUnitFormatterClass); });
    return __kCUnitFormatterTypeID;
}




CFUnitFormatterRef CFUnitFormatterCreate(CFAllocatorRef allocator,
                                         CFLocaleRef locale,
                                         CFUnitFormatterStyle  ustyle,
                                         CFNumberFormatterStyle cfnfms){
    struct __CFUnitFormatter *memory;
    uint32_t size = sizeof(struct __CFUnitFormatter) - sizeof(CFRuntimeBase);
    if (allocator == NULL) allocator = __CFGetDefaultAllocator();
    __CFGenericValidateType(allocator, CFAllocatorGetTypeID());
    __CFGenericValidateType(locale, CFLocaleGetTypeID());
    memory = (struct __CFUnitFormatter *)_CFRuntimeCreateInstance(allocator, CFUnitFormatterGetTypeID(), size, NULL);
    if (NULL == memory) {
        return NULL;
    }
    
    memory->_mf = NULL;
    memory->_locale = NULL;
    memory->_pattern = NULL;
    memory->_rules = NULL;
    if (NULL == locale) locale = CFLocaleGetSystem();
    
    uint32_t ufstyle;
    switch (ustyle) {
        case kCFUnitFormatterNumericStyle: ustyle = UAMEASFMT_WIDTH_NUMERIC; break;
        case kCFUnitFormatterShortStyle: ustyle = UAMEASFMT_WIDTH_NARROW; break;
        case kCFUnitFormatterMediumStyle: ustyle = UAMEASFMT_WIDTH_SHORT; break;
        case kCFUnitFormatterLongStyle: ustyle = UAMEASFMT_WIDTH_WIDE; break;
        default:
            CFAssert(0, __kCFLogAssertion, "%s(): unknown style %d", __PRETTY_FUNCTION__, ufstyle);
            ufstyle = UAMEASFMT_WIDTH_SHORT;
            memory->_ufstyle = kCFUnitFormatterMediumStyle;
            break;
    }
    
    
    
    CFStringRef localeName = locale ? CFLocaleGetIdentifier(locale) : CFSTR("");
    char buffer[BUFFER_SIZE];
    const char *cstr = CFStringGetCStringPtr(localeName, kCFStringEncodingASCII);
    printf("%s", cstr);

    if (NULL == cstr) {
        if (CFStringGetCString(localeName, buffer, BUFFER_SIZE, kCFStringEncodingASCII)) cstr = buffer;
    }
    
    if (NULL == cstr) {
        CFRelease(memory);
        return NULL;
    }
    
    UErrorCode status = U_ZERO_ERROR;
    uint32_t icustyle;

    switch (cfnfms) {
        case kCFNumberFormatterNoStyle: ustyle = UNUM_IGNORE; break;
        case kCFNumberFormatterDecimalStyle: ustyle = UNUM_DECIMAL; break;
        case kCFNumberFormatterCurrencyStyle: ustyle = UNUM_CURRENCY; break;
        case kCFNumberFormatterPercentStyle: ustyle = UNUM_PERCENT; break;
        case kCFNumberFormatterScientificStyle: ustyle = UNUM_SCIENTIFIC; break;
        case kCFNumberFormatterSpellOutStyle: ustyle = UNUM_SPELLOUT; break;
        case kCFNumberFormatterOrdinalStyle: ustyle = UNUM_ORDINAL; break;
        case kCFNumberFormatterDurationStyle: ustyle = UNUM_DURATION; break;
        case kCFNumberFormatterCurrencyISOCodeStyle: ustyle = UNUM_CURRENCY_ISO; break;
        case kCFNumberFormatterCurrencyPluralStyle: ustyle = UNUM_CURRENCY_PLURAL; break;
#if U_ICU_VERSION_MAJOR_NUM >= 55
        case kCFNumberFormatterCurrencyAccountingStyle: ustyle = UNUM_CURRENCY_ACCOUNTING; break;
#endif
        default:
            CFAssert(0, __kCFLogAssertion, "%s(): unknown style %d", __PRETTY_FUNCTION__, icustyle);
            icustyle = UNUM_DECIMAL;
            break;
    }
    
    memory->_unf = __cficu_unum_open(icustyle, NULL, -1, cstr, NULL, &status);

    CFAssert(memory->_unf, __kCFLogAssertion, "%s(): error (%d) creating number formatter", __PRETTY_FUNCTION__, status);

    UErrorCode ustatus = U_ZERO_ERROR;
    
    memory->_mf = __cficu_uameasfmt_open(cstr,(CFUnitFormatterStyle)ufstyle, memory->_unf, &ustatus) ;
    CFAssert(memory->_mf, __kCFLogAssertion, "%s(): error (%d) creating number formatter", __PRETTY_FUNCTION__, ustatus);

    
    if (NULL == memory->_unf || NULL == memory->_mf) {
        CFRelease(memory);
        return NULL;
    }
    
    return (CFUnitFormatterRef)memory;
}


CFStringRef CGUnitFormatterPluralUnitMetric(CFUnitFormatterRef formatter,
                                            struct CFMeasure*  measures){
    UAMeasure m;
    m.value = 12;
    m.unit = UAMEASUNIT_ELECTRIC_VOLT;
    const UAMeasure measure[1] = { m };
    UChar buffer[BUFFER_SIZE];
    UErrorCode  MultiErrStatus = U_ZERO_ERROR;
    
    int32_t leng =  uameasfmt_formatMultiple(formatter->_mf,
                                             measure,
                                             1, //count
                                             buffer,
                                             BUFFER_SIZE,
                                             &MultiErrStatus);
    
    
    
    
    
    if (U_SUCCESS(MultiErrStatus) && leng <= BUFFER_SIZE) {
        CFStringRef r = CFStringCreateWithCharacters(kCFAllocatorDefault, (const UniChar *)buffer, leng);
        return  r;
        myShow(r);
    }
    return NULL;
}




