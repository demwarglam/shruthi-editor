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

#include <QtGui> 
#include "shruthi-editor.h"
#include "settings-dialog.h"

#include "lib_labels.h"
#include "lib_patch.h"

#define VERSION "0.17"

Editor* editor;
int MIDI_INPUT_DEVICE, MIDI_OUTPUT_DEVICE; 
bool MIDI_INPUT_STATUS, MIDI_OUTPUT_STATUS;

// ******************************************
shruthiEditorMainWindow::shruthiEditorMainWindow(Editor* edit, QWidget *parent) {
// ******************************************
    setupUi(this); 
    editor = edit;

    // Setup Dials/ComboBoxes:
    QDial* tmp_d;
    QComboBox* tmp_c;
    for (int i=0; i<100; i++) if (Patch::enabled(i)) {
        if (Patch::parameters[i].dropdown) {
            tmp_c = qFindChild<QComboBox*>(this, QString("c")+QString("%1").arg(i));
            tmp_c -> addItems(*(Patch::parameters[i].dropdown));
            connect(tmp_c,SIGNAL(currentIndexChanged(int)),this,SLOT(comboBoxChanged(int)));
        } else {
            tmp_d = qFindChild<QDial*>(this, QString("c")+QString("%1").arg(i));
            tmp_d->setMinimum(Patch::parameters[i].min);
            tmp_d->setMaximum(Patch::parameters[i].max);
            qFindChild<QLabel*>(this, QString("d")+QString("%1").arg(i))->setText("0");
            connect (tmp_d,SIGNAL(valueChanged(int)), this, SLOT(dialChanged(int)));
        }
    }
    connect(patch_name,SIGNAL(editingFinished()),this, SLOT(patchNameChanged()));

    // Now that everything is set up, update all UI elements:
    redrawAll();
    
    // other UI Signals:
    connect(actionLoad_Patch,SIGNAL(triggered()), this, SLOT(loadPatch()));
    connect(actionSave_Patch,SIGNAL(triggered()), this, SLOT(savePatch()));
    connect(actionFetch_Patch,SIGNAL(triggered()), this, SLOT(fetchPatch()));
    connect(actionSend_Patch,SIGNAL(triggered()), this, SLOT(sendPatch()));
    connect(actionChange_Midi_Ports,SIGNAL(triggered()), this, SLOT(changeMidiPorts()));
    connect(actionReset_Patch,SIGNAL(triggered()), this, SLOT(resetPatch()));
    connect(actionQuit,SIGNAL(triggered()), this, SLOT(quitShruthiEditor()));
    connect(actionAbout_Shruthi_Editor,SIGNAL(triggered()), this, SLOT(aboutShruthiEditor()));
    connect(actionAbout_Qt,SIGNAL(triggered()), this, SLOT(aboutQt()));
    connect(actionRandomize_Patch,SIGNAL(triggered()), this, SLOT(randomizePatch()));
}

void shruthiEditorMainWindow::setMidiDevices(int midiin, int midiout) {
#ifdef DEBUG
    qDebug() << "shruthiEditorMainWindow::setMidiDevices";
#endif
    MIDI_INPUT_DEVICE = midiin;
    MIDI_OUTPUT_DEVICE = midiout;
}

// ******************************************
// ******************************************
// Local UI Signal Handlers
// ******************************************
// ******************************************

// ******************************************
void shruthiEditorMainWindow::comboBoxChanged(int val) {
// ******************************************
    QComboBox* s = (QComboBox*) sender();
    QString id = s->objectName();
    id.remove(0,1);
    queueitem_t signal (NRPN_PROCESS_EDITOR,id.toInt(),val);
    emit(enqueue(signal));
}

// ******************************************
void shruthiEditorMainWindow::dialChanged(int val) {
// ******************************************
    QDial* s = (QDial*) sender();
    QString id = s->objectName();
    id.replace(0,1,"d");
    if (id=="d25" || id == "d29")
        qFindChild<QLabel*>(this, id)->setText(Labels::LfoRateFormatter(val));
    else
        qFindChild<QLabel*>(this, id)->setText(QString("%1").arg(val));
    id.remove(0,1);
    queueitem_t signal (NRPN_PROCESS_EDITOR,id.toInt(),val);
    emit(enqueue(signal));
}

// ******************************************
void shruthiEditorMainWindow::patchNameChanged() {
// ******************************************
    QString name = patch_name->text();
    queueitem_t signal (SET_PATCHNAME,patch_name->text());
    emit enqueue(signal);
}

// ******************************************
void shruthiEditorMainWindow::loadPatch() {
// ******************************************
    QString filename = QFileDialog::getOpenFileName(this, "Open patch", "*.sp", "Shruthi-Patches(*.sp)");
    if (filename!="") {
        queueitem_t signal (FILEIO_LOAD,filename);
        emit(enqueue(signal));        
    }
}

// ******************************************
void shruthiEditorMainWindow::savePatch() {
// ******************************************
    QString filename = QFileDialog::getSaveFileName(this, "Save patch", ".sp", "Shruthi-Patches(*.sp)");
    if (filename!="") {
        queueitem_t signal (FILEIO_SAVE,filename);
        emit(enqueue(signal));      
    }
}

// ******************************************
void shruthiEditorMainWindow::fetchPatch() {
// ******************************************
    queueitem_t signal (SYSEX_FETCH_PATCH);
    emit(enqueue(signal));   
}

// ******************************************
void shruthiEditorMainWindow::sendPatch() {
// ******************************************
    queueitem_t signal (SYSEX_SEND_PATCH);
    emit(enqueue(signal));   
}

// ******************************************
void shruthiEditorMainWindow::changeMidiPorts() {
// ******************************************
    shruthiEditorSettings prefs;
    prefs.setChannels(MIDI_INPUT_DEVICE, MIDI_OUTPUT_DEVICE);
    if (prefs.exec()) {
        int in = prefs.getInputChannel();
        int out = prefs.getOutputChannel(); 
        emit midiDeviceChanged(in,out);
    }
    prefs.done(1);
}

// ******************************************
void shruthiEditorMainWindow::resetPatch() {
// ******************************************
    queueitem_t signal (RESET_PATCH);
    emit(enqueue(signal));
}

// ******************************************
void shruthiEditorMainWindow::randomizePatch() {
// ******************************************
    queueitem_t signal (RANDOMIZE_PATCH);
    emit(enqueue(signal));
}


// ******************************************
void shruthiEditorMainWindow::quitShruthiEditor() {
// ******************************************
    QApplication::exit( 1);
}


// ******************************************
void shruthiEditorMainWindow::aboutShruthiEditor() {
// ******************************************
    QMessageBox::about(this,"About Shruthi-Editor",
        QString::fromUtf8("Shruti-Editor Version "VERSION".\n\n Copyright (C) 2011 Manuel Krönig."));
}

// ******************************************
void shruthiEditorMainWindow::aboutQt() {
// ******************************************
    QMessageBox::aboutQt(this);
}

// ******************************************
// ******************************************
// Redraw Signal Handlers
// ******************************************
// ******************************************

// ******************************************
void shruthiEditorMainWindow::redrawNRPN(int nrpn) {
// ******************************************
    if (Patch::parameters[nrpn].dropdown) {
        qFindChild<QComboBox*>(this, QString("c")+QString("%1").arg(nrpn))
          -> setCurrentIndex(editor->getParam(nrpn));
    } else {
        qFindChild<QDial*>(this, QString("c")+QString("%1").arg(nrpn))
          -> setValue(editor->getParam(nrpn));
    }
}

// ******************************************
void shruthiEditorMainWindow::redrawAll() {
// ******************************************
    for (int i=0; i<100; i++) if (Patch::enabled(i))
        redrawNRPN(i);
    patch_name->setText(editor->getName());
}


// ******************************************
// ******************************************
// Statusbar
// ******************************************
// ******************************************

// ******************************************
void shruthiEditorMainWindow::midiInputStatusChanged(bool st) {
// ******************************************
    MIDI_INPUT_STATUS=st;
    displayMidiStatusChanged(st, MIDI_OUTPUT_STATUS);
}

// ******************************************
void shruthiEditorMainWindow::midiOutputStatusChanged(bool st) {
// ******************************************
    MIDI_OUTPUT_STATUS=st;
    displayMidiStatusChanged(MIDI_INPUT_STATUS,st);
}

// ******************************************
void shruthiEditorMainWindow::displayMidiStatusChanged(bool in, bool out) {
// ******************************************
    QString status = "MidiIn: ";
    if (!in)
        status += "not ";
    status += "ready. MidiOut: ";
    if (!out)
        status += "not ";
    status += "ready.";
    statusBar()->showMessage(status);
}

// ******************************************
void shruthiEditorMainWindow::displayStatusbar(QString msg) {
// ******************************************
    statusBar()->showMessage(msg);
}