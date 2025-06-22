/*
 * Copyright (C) 2016 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * Multi-language support for FreeMiNT
 * Based on Atari TOS language codes
 */

#include <langinfo.h>
#include <stdlib.h>
#include <mint/cookie.h>  // For cookie jar access if available

// Language codes from TOS
#define LANG_USA  0
#define LANG_FRG  1  // Germany
#define LANG_FRA  2  // France
#define LANG_UK   3
#define LANG_SPA  4  // Spain
#define LANG_ITA  5  // Italy
#define LANG_SWE  6  // Sweden
#define LANG_SWF  7  // Swiss French
#define LANG_SWG  8  // Swiss German
#define LANG_TUR  9  // Turkey
#define LANG_FIN  10 // Finland
#define LANG_NOR  11 // Norway
#define LANG_DEN  12 // Denmark
#define LANG_SAU  13 // Saudi Arabia
#define LANG_HOL  14 // Netherlands

// Language-specific string tables
typedef struct {
    const char* codeset;
    const char* d_t_fmt;
    const char* d_fmt;
    const char* t_fmt;
    const char* t_fmt_ampm;
    const char* am_str;
    const char* pm_str;
    const char* days[7];        // Sunday to Saturday
    const char* abdays[7];      // Abbreviated days
    const char* months[12];     // January to December
    const char* abmonths[12];   // Abbreviated months
    const char* radixchar;
    const char* thousep;
    const char* yesexpr;
    const char* noexpr;
    const char* currency;
} lang_data_t;

// Language data table
static const lang_data_t lang_table[] = {
    // USA (English)
    {
        .codeset = "UTF-8",
        .d_t_fmt = "%a %b %e %H:%M:%S %Z %Y",
        .d_fmt = "%m/%d/%y",
        .t_fmt = "%H:%M:%S",
        .t_fmt_ampm = "%I:%M:%S %p",
        .am_str = "AM", .pm_str = "PM",
        .days = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"},
        .abdays = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"},
        .months = {"January", "February", "March", "April", "May", "June",
                   "July", "August", "September", "October", "November", "December"},
        .abmonths = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                     "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"},
        .radixchar = ".", .thousep = ",",
        .yesexpr = "^[yY]", .noexpr = "^[nN]",
        .currency = "$"
    },
    // FRG (German)
    {
        .codeset = "UTF-8",
        .d_t_fmt = "%a %d %b %Y %T %Z",
        .d_fmt = "%d.%m.%Y",
        .t_fmt = "%T",
        .t_fmt_ampm = "%I:%M:%S %p",
        .am_str = "AM", .pm_str = "PM",
        .days = {"Sonntag", "Montag", "Dienstag", "Mittwoch", "Donnerstag", "Freitag", "Samstag"},
        .abdays = {"So", "Mo", "Di", "Mi", "Do", "Fr", "Sa"},
        .months = {"Januar", "Februar", "März", "April", "Mai", "Juni",
                   "Juli", "August", "September", "Oktober", "November", "Dezember"},
        .abmonths = {"Jan", "Feb", "Mär", "Apr", "Mai", "Jun",
                     "Jul", "Aug", "Sep", "Okt", "Nov", "Dez"},
        .radixchar = ",", .thousep = ".",
        .yesexpr = "^[jJyY]", .noexpr = "^[nN]",
        .currency = "€"
    },
    // FRA (French)
    {
        .codeset = "UTF-8",
        .d_t_fmt = "%a %d %b %Y %T %Z",
        .d_fmt = "%d/%m/%Y",
        .t_fmt = "%T",
        .t_fmt_ampm = "%I:%M:%S %p",
        .am_str = "AM", .pm_str = "PM",
        .days = {"dimanche", "lundi", "mardi", "mercredi", "jeudi", "vendredi", "samedi"},
        .abdays = {"dim", "lun", "mar", "mer", "jeu", "ven", "sam"},
        .months = {"janvier", "février", "mars", "avril", "mai", "juin",
                   "juillet", "août", "septembre", "octobre", "novembre", "décembre"},
        .abmonths = {"jan", "fév", "mar", "avr", "mai", "jun",
                     "jul", "aoû", "sep", "oct", "nov", "déc"},
        .radixchar = ",", .thousep = " ",
        .yesexpr = "^[oOyY]", .noexpr = "^[nN]",
        .currency = "€"
    },
    // UK (English)
    {
        .codeset = "UTF-8",
        .d_t_fmt = "%a %d %b %Y %T %Z",
        .d_fmt = "%d/%m/%y",
        .t_fmt = "%T",
        .t_fmt_ampm = "%I:%M:%S %p",
        .am_str = "AM", .pm_str = "PM",
        .days = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"},
        .abdays = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"},
        .months = {"January", "February", "March", "April", "May", "June",
                   "July", "August", "September", "October", "November", "December"},
        .abmonths = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                     "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"},
        .radixchar = ".", .thousep = ",",
        .yesexpr = "^[yY]", .noexpr = "^[nN]",
        .currency = "£"
    },
    // SPA (Spanish)
    {
        .codeset = "UTF-8",
        .d_t_fmt = "%a %d %b %Y %T %Z",
        .d_fmt = "%d/%m/%Y",
        .t_fmt = "%T",
        .t_fmt_ampm = "%I:%M:%S %p",
        .am_str = "AM", .pm_str = "PM",
        .days = {"domingo", "lunes", "martes", "miércoles", "jueves", "viernes", "sábado"},
        .abdays = {"dom", "lun", "mar", "mié", "jue", "vie", "sáb"},
        .months = {"enero", "febrero", "marzo", "abril", "mayo", "junio",
                   "julio", "agosto", "septiembre", "octubre", "noviembre", "diciembre"},
        .abmonths = {"ene", "feb", "mar", "abr", "may", "jun",
                     "jul", "ago", "sep", "oct", "nov", "dic"},
        .radixchar = ",", .thousep = ".",
        .yesexpr = "^[sSyY]", .noexpr = "^[nN]",
        .currency = "€"
    },
    // ITA (Italian)
    {
        .codeset = "UTF-8",
        .d_t_fmt = "%a %d %b %Y %T %Z",
        .d_fmt = "%d/%m/%Y",
        .t_fmt = "%T",
        .t_fmt_ampm = "%I:%M:%S %p",
        .am_str = "AM", .pm_str = "PM",
        .days = {"domenica", "lunedì", "martedì", "mercoledì", "giovedì", "venerdì", "sabato"},
        .abdays = {"dom", "lun", "mar", "mer", "gio", "ven", "sab"},
        .months = {"gennaio", "febbraio", "marzo", "aprile", "maggio", "giugno",
                   "luglio", "agosto", "settembre", "ottobre", "novembre", "dicembre"},
        .abmonths = {"gen", "feb", "mar", "apr", "mag", "giu",
                     "lug", "ago", "set", "ott", "nov", "dic"},
        .radixchar = ",", .thousep = ".",
        .yesexpr = "^[sSyY]", .noexpr = "^[nN]",
        .currency = "€"
    }
    // Add more languages as needed...
};

#define NUM_LANGUAGES (sizeof(lang_table) / sizeof(lang_table[0]))

// Function to detect current language from TOS
static int get_current_language(void) {

    long cookie_val;
    if (Getcookie(C__AKP, &cookie_val) == C_FOUND) {
        return (cookie_val >> 8) & 0xff;  // Language is in bits 8-15
    }

    return LANG_USA;
}

// Get language data for current language
static const lang_data_t* get_lang_data(void) {
    int lang = get_current_language();
    if (lang >= 0 && lang < NUM_LANGUAGES) {
        return &lang_table[lang];
    }
    return &lang_table[LANG_USA];  // Default to USA
}

char* nl_langinfo(nl_item item) {
    const lang_data_t* lang = get_lang_data();
    const char* result = "";
    
    switch (item) {
        case CODESET: result = lang->codeset; break;
        case D_T_FMT: result = lang->d_t_fmt; break;
        case D_FMT: result = lang->d_fmt; break;
        case T_FMT: result = lang->t_fmt; break;
        case T_FMT_AMPM: result = lang->t_fmt_ampm; break;
        case AM_STR: result = lang->am_str; break;
        case PM_STR: result = lang->pm_str; break;
        
        // Days of week
        case DAY_1: result = lang->days[0]; break;  // Sunday
        case DAY_2: result = lang->days[1]; break;  // Monday
        case DAY_3: result = lang->days[2]; break;
        case DAY_4: result = lang->days[3]; break;
        case DAY_5: result = lang->days[4]; break;
        case DAY_6: result = lang->days[5]; break;
        case DAY_7: result = lang->days[6]; break;  // Saturday
        
        // Abbreviated days
        case ABDAY_1: result = lang->abdays[0]; break;
        case ABDAY_2: result = lang->abdays[1]; break;
        case ABDAY_3: result = lang->abdays[2]; break;
        case ABDAY_4: result = lang->abdays[3]; break;
        case ABDAY_5: result = lang->abdays[4]; break;
        case ABDAY_6: result = lang->abdays[5]; break;
        case ABDAY_7: result = lang->abdays[6]; break;
        
        // Months
        case MON_1: result = lang->months[0]; break;   // January
        case MON_2: result = lang->months[1]; break;
        case MON_3: result = lang->months[2]; break;
        case MON_4: result = lang->months[3]; break;
        case MON_5: result = lang->months[4]; break;
        case MON_6: result = lang->months[5]; break;
        case MON_7: result = lang->months[6]; break;
        case MON_8: result = lang->months[7]; break;
        case MON_9: result = lang->months[8]; break;
        case MON_10: result = lang->months[9]; break;
        case MON_11: result = lang->months[10]; break;
        case MON_12: result = lang->months[11]; break;
        
        // Abbreviated months
        case ABMON_1: result = lang->abmonths[0]; break;
        case ABMON_2: result = lang->abmonths[1]; break;
        case ABMON_3: result = lang->abmonths[2]; break;
        case ABMON_4: result = lang->abmonths[3]; break;
        case ABMON_5: result = lang->abmonths[4]; break;
        case ABMON_6: result = lang->abmonths[5]; break;
        case ABMON_7: result = lang->abmonths[6]; break;
        case ABMON_8: result = lang->abmonths[7]; break;
        case ABMON_9: result = lang->abmonths[8]; break;
        case ABMON_10: result = lang->abmonths[9]; break;
        case ABMON_11: result = lang->abmonths[10]; break;
        case ABMON_12: result = lang->abmonths[11]; break;
        
        // Other locale info
        case RADIXCHAR: result = lang->radixchar; break;
        case THOUSEP: result = lang->thousep; break;
        case YESEXPR: result = lang->yesexpr; break;
        case NOEXPR: result = lang->noexpr; break;
        case CRNCYSTR: result = lang->currency; break;
        
        // Era info (not supported)
        case ERA:
        case ERA_D_FMT:
        case ERA_D_T_FMT:
        case ERA_T_FMT:
        case ALT_DIGITS:
        default:
            result = "";
            break;
    }
    
    return (char*)result;
}

char* nl_langinfo_l(nl_item item, locale_t l) {
    // For now, ignore locale parameter and use system default
    // Could be enhanced to support specific locales
    return nl_langinfo(item);
}