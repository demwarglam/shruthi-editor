// Shruthi-Editor: An unofficial Editor for the Shruthi hardware synthesizer. For 
// informations about the Shruthi, see <http://www.mutable-instruments.net/shruthi1>. 
//
// Copyright (C) 2011 Manuel Krönig
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef SHRUTHI_PATCH_H
#define SHRUTHI_PATCH_H
#include "lib_labels.h"
#include <vector>

// ******************************************
struct param_t {
// ******************************************
    QString name;
    int min;
    int max;
    QStringList* dropdown;
};

// ******************************************
class Patch {
// ******************************************
    private:
        QString name;
        int data[108];
        unsigned char calculateChecksum(unsigned char sysex[], unsigned int start, unsigned int end);
        void parseSysex(unsigned char *sysex);
        void generateSysex(unsigned char res[]);
        static const unsigned char sysexHead[6];
        static const unsigned char sysexFoot;
    public:
        static param_t parameters[];
        static unsigned char INIT_PATCH[];
        static bool enabled(int);
        
        Patch();
        
        void setParam(int param, int value);
        int getParam(int param);
        void setName(QString name);
        QString getName();

        void printPatch();
        void resetPatch();
        void randomizePatch();
        void parseFullSysex(unsigned char  *sysex, unsigned int len);
        void parseFullSysex(std::vector<unsigned char> message);
        void generateFullSysex(std::vector<unsigned char> *message);

        bool loadFromDisk(QString location);
        bool saveToDisk(QString location);
};
#endif
