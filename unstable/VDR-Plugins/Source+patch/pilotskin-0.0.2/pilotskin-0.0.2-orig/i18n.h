/*
 * Pilot plugin to VDR (C++)
 *
 * (C) 2001,2002 Olivier Jacques (vdr@olivierjacques.com)
 *
 * This code is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 * Or, point your browser to http://www.gnu.org/copyleft/gpl.html
 */

#ifndef ___I18N_H
#define ___I18N_H

#include <vdr/i18n.h>
#ifndef VDRVERSNUM
#include <vdr/config.h>
#endif

extern const char *i18n_name;
extern const tI18nPhrase Phrases[];

#undef tr
#define tr(s)  I18nTranslate(s, i18n_name)

const char *i18n_name = 0;

const tI18nPhrase Phrases[] = {
/*
 * French     translations provided by Maxime Guilbot <maxime.guilbot@orange.fr>
*/
  { "Pilotskin", // ENGLISH
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Pilotskin", // FR
    "", // TODO
    "Kanavapilotti", // FINNISH
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
#if VDRVERSNUM>10301
    "", // TODO
#endif
  },
  { "A zapping co-pilot", // ENGLISH
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Co-pilote de zapping", // FR
    "", // TODO
    "Kanavapilotti ohjelmaoppaalle", // FINNISH
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
#if VDRVERSNUM>10301
    "", // TODO
#endif
  },
  { "Red", // ENGLISH
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Rouge", // FR
    "", // TODO
    "Punainen", // FINNISH
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
#if VDRVERSNUM>10301
    "", // TODO
#endif
  },
  { "Green", // ENGLISH                                                                                 
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Vert", // FR
    "", // TODO
    "Vihre�", // FINNISH 
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
#if VDRVERSNUM>10301
    "", // TODO
#endif
  },
  { "Blue", // ENGLISH                                                                                 
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Bleu", // FR
    "", // TODO
    "Sininen", // FINNISH
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
#if VDRVERSNUM>10301
    "", // TODO
#endif
  },
  { "Channel", // ENGLISH
    "Kanal", // GERMAN
    "", // TODO
    "Canale", // ITALIAN
    "Kanaal", // NL
    "", // TODO
    "Cha�nes", // FR
    "", // TODO
    "Kanava", // FINNISH
    "", // TODO
    "", // TODO
    "Czatoma", // HU
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
#if VDRVERSNUM>10301
    "", // TODO
#endif
  },
  { "Origin X", // EN
    "Position X", // GERMAN
    "", // TODO
    "Origine X", // ITALIAN
    "X positie", // NL
    "", // TODO
    "Origine X", // FR
    "", // TODO
    "Vaakasijainti", // FINNISH
    "", // TODO
    "", // TODO
    "X helyzet", // HU
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
#if VDRVERSNUM>10301
    "", // TODO
#endif
  },
  { "Origin Y", // EN
    "Position Y", // GERMAN
    "", // TODO
    "Origine Y", // ITALIAN 
    "Y positie", // NL
    "", // TODO
    "Origine Y", // FR
    "", // TODO
    "Pystysijainti", // FINNISH
    "", // TODO
    "", // TODO
    "Y helyzet", // HU
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
#if VDRVERSNUM>10301
    "", // TODO
#endif
  },
  { "Width", //EN
    "Breite", // GER
    "", // TODO
    "Larghezza", // ITALIAN
    "Breedte", // NL
    "",
    "Largeur", 
    "", // TODO
    "Leveys", // FINNISH
    "", // TODO
    "", // TODO
    "Sz�less�g", // HU
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
#if VDRVERSNUM>10301
    "", // TODO
#endif
  },
  { "AddChannel",
    "Kanal hinzuf�gen", // GER
    "", // TODO
    "Aggiungi Canale", // ITALIAN
    "iKanaal bijdoen", // NL
    "", // TODO
    "Nouvelle cha�ne",  // FR
    "", // TODO
    "Lis�� kanava", // FINNISH
    "", // TODO
    "", // TODO
    "Adj csatorn�t hozza", // HU
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
#if VDRVERSNUM>10301
    "", // TODO
#endif
  },
  { "RemoveChannel",
    "Kanal l�schen", // GER
    "", // TODO
    "Rimuovi Canale", // ITALIAN
    "Kanaal wegneemen", // NL
    "", // TODO
    "Supprime cha�ne", //FR
    "", // TODO
    "Poista kanava", // FINNISH
    "", // TODO
    "", // TODO
    "Fok csatorn�t", // HU
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
#if VDRVERSNUM>10301
    "", // TODO
#endif
  },
  { "Background opacity",
    "Rand transparenz", // TODO
    "", // TODO
    "Alpha Bordo 1", // ITALIAN
    "", // TODO
    "", // TODO
    "Opacit� du fond",
    "", // TODO
    "Taustan l�pin�kyvyys", // FINNISH
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
#if VDRVERSNUM>10301
    "", // TODO
#endif
  },
  { "Text opacity",
    "", // TODO
    "", // TODO
    "Alpha Bordo 2", // ITALIAN
    "", // TODO
    "", // TODO
    "Opacit� du texte",
    "", // TODO
    "Tekstin l�pin�kyvyys", // FINNISH
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
#if VDRVERSNUM>10301
    "", // TODO
#endif
  },
  { "Border opacity",
    "Alpha Rand", // GER
    "", // TODO
    "AlphaBordo", // ITALIAN
    "Alpha kant", // NL
    "", // TODO
    "Opacit� du contour", // FR
    "", // TODO
    "Reunan l�pin�kyvyys", // FINNISH
    "", // TODO
    "", // TODO
    "Alpha szeg�ly", // HU
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
#if VDRVERSNUM>10301
    "", // TODO
#endif
  },
  { "Sort by",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Trier par",
    "", // TODO
    "J�rjest�", // FINNISH
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
#if VDRVERSNUM>10301
    "", // TODO
#endif
  },
  { "None",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Aucun",
    "", // TODO
    "ei", // FINNISH
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
#if VDRVERSNUM>10301
    "", // TODO
#endif
  },
  { "Name",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Nom",
    "", // TODO
    "nimen mukaan", // FINNISH
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
#if VDRVERSNUM>10301
    "", // TODO
#endif
  },
  { "As entered",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Tel que saisi",
    "", // TODO
    "sy�tteen mukaan", // FINNISH
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
#if VDRVERSNUM>10301
    "", // TODO
#endif
  },
  { "Close on OK",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Fermer le menu sur OK",
    "", // TODO
    "Sulje painamalla OK", // FINNISH
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
#if VDRVERSNUM>10301
    "", // TODO
#endif
  },
  { "Number of channels",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Nombre de cha�nes",
    "", // TODO
    "Kanavien m��r�", // FINNISH
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
#if VDRVERSNUM>10301
    "", // TODO
#endif
  },
  { "Hide main menu entry", // English
    "Hauptmen�eintrag verstecken", // Deutsch
    "", // Slovenski
    "Nascondi voce men�", // Italiano
    "", // Nederlands
    "", // Portugu�s
    "Masquer dans le menu principal", // Fran�ais
    "", // Norsk
    "Piilota valinta p��valikosta", // suomi
    "", // Polski
    "Ocultar en el men� principal", // Espa�ol
    "", // �������� (Greek)
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catal�
#if VDRVERSNUM>10301
    "", // TODO
#endif
  },
  { "Button$Close",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Fermer", // FR
    "", // TODO
    "", // FINNISH
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
#if VDRVERSNUM>10301
    "", // TODO
#endif
  },
 { NULL }
};

#endif //___I18N_H
