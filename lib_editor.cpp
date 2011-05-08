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

#include "lib_editor.h"
#include <QDebug>

// ******************************************
unsigned char Editor::transferRequest[]={240,0,32,119,0,2,17,0,0,0,247};
// ******************************************

// ******************************************
Editor::Editor() {
// ******************************************
#ifdef DEBUG
    qDebug("Editor::Editor()");
#endif
};

// ******************************************
bool Editor::setMidiDevices(int in, int out) {
// ******************************************
#ifdef DEBUG
    qDebug() << "Editor::setMidiDevices:" << out;
#endif
    bool status = midiout.open(out);
    emit midiOutputStatusChanged(status);
    return status;
}

// ******************************************
Editor::~Editor() {
// ******************************************
#ifdef DEBUG
    qDebug() << "Editor::~Editor()";
#endif
    midiout.close();
}

// ******************************************
void Editor::process(queueitem_t item) {
// ******************************************
    bool status;
    switch (item.action) {
        case NRPN_PROCESS_EDITOR:
#ifdef DEBUG
            qDebug() << "NRPN_PROCESS_EDITOR: "<<item.int0 << ":" <<item.int1;
#endif
            if (patch.getParam(item.int0) != item.int1) {
                patch.setParam(item.int0,item.int1);
                midiout.writeNRPN(item.int0,item.int1);
            }
            break;
         case SYSEX_FETCH_PATCH:
#ifdef DEBUG
             qDebug() << "SYSEX_FETCH_PATCH";
#endif
             midiout.writeSysex(Editor::transferRequest);
             break;
        case SYSEX_SEND_PATCH:
#ifdef DEBUG
            qDebug() << "SYSEX_SEND_PATCH";
#endif
            unsigned char temp[195];
            patch.generateFullSysex(temp);
            midiout.writeSysex(temp);
            break;
        case NRPN_RECIEVED:
            if (Patch::parameters[item.int0].min<0 && item.int1>=127)
                item.int1-=256; //2s complement
            patch.setParam(item.int0,item.int1);
            emit(redrawNRPN(item.int0));
#ifdef DEBUG
            qDebug() << "NRPN_RECIEVED:" <<item.int0<<":"<<item.int1;
#endif
            break;
        case SYSEX_RECIEVED:
#ifdef DEBUG
            qDebug() << "SYSEX_RECIEVED";
#endif
            if (7<item.int0 && item.intp[6]==1 && item.intp[7]==0)
                patch.parseFullSysex(item.intp,item.int0);
#ifdef DEBUG
            else
                qDebug() << "unknown sysex type";
#endif
            delete item.intp;
            emit redrawAll();
            break;
        case SET_PATCHNAME:
#ifdef DEBUG
            qDebug() << "SET_PATCHNAME: " << item.string;
#endif
            patch.setName(item.string);
            break; 
        case FILEIO_LOAD:
            status = patch.loadFromDisk(item.string);
            
#ifdef DEBUG
            qDebug() << "FILEIO_LOAD: " << item.string << ":" << status;
#endif
            emit redrawAll();
            break;
        case FILEIO_SAVE:
            status = patch.saveToDisk(item.string);
#ifdef DEBUG
            qDebug() << "FILEIO_SAVE: " << item.string << ":" << status;
#endif
            break;
        case RESET_PATCH:
#ifdef DEBUG
            qDebug() << "RESET_PATCH";
#endif
            patch.resetPatch();
            emit redrawAll();
            break;
        case RANDOMIZE_PATCH:
#ifdef DEBUG
            qDebug() << "RANDOMIZE_PATCH";
#endif
            patch.randomizePatch();
            emit redrawAll();
            break;
        default:
#ifdef DEBUG
            qDebug() << "dequeue: " << item.action << ":" << item.int0 << "," << item.int1 << "," << item.string;
#endif
            break;
    }
    emit finished();
};

// ******************************************
int Editor::getParam(int nrpn) {
// ******************************************
    return patch.getParam(nrpn);
};

// ******************************************
QString Editor::getName() {
// ******************************************
    return patch.getName();
};