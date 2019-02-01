/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

//! [0]
#include <QtWidgets>

#include "editeur.h"
//! [0]

//! [1]
Editeur::Editeur()
    : champTexte(new QPlainTextEdit)
//! [1] //! [2]
{
    setCentralWidget(champTexte);

    creerActions();
    creerBarreEtat();

    lireOptions();

    connect(champTexte->document(), &QTextDocument::contentsChanged,
            this, &Editeur::documentWasModified);

#ifndef QT_NO_SESSIONMANAGER
    QGuiApplication::setFallbackSessionManagementEnabled(false);
    connect(qApp, &QGuiApplication::commitDataRequest,
            this, &Editeur::commitData);
#endif

    setFichier(QString());
    setUnifiedTitleAndToolBarOnMac(true);
}
//! [2]

//! [3]
void Editeur::closeEvent(QCloseEvent *event)
//! [3] //! [4]
{
    if (sauvegarderPeutEtre()) {
        ecrireOptions();
        event->accept();
    } else {
        event->ignore();
    }
}
//! [4]

//! [5]
void Editeur::creer()
//! [5] //! [6]
{
    if (sauvegarderPeutEtre()) {
        champTexte->clear();
        setFichier(QString());
    }
}
//! [6]

//! [7]
void Editeur::ouvrir()
//! [7] //! [8]
{
    if (sauvegarderPeutEtre()) {
        QString fileName = QFileDialog::getOpenFileName(this);
        if (!fileName.isEmpty())
            chargerFichier(fileName);
    }
}
//! [8]

//! [9]
bool Editeur::sauvegarder()
//! [9] //! [10]
{
    if (fichier.isEmpty()) {
        return sauvegarderSous();
    } else {
        return sauvegarderFichier(fichier);
    }
}
//! [10]

//! [11]
bool Editeur::sauvegarderSous()
//! [11] //! [12]
{
    QFileDialog dialog(this);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    if (dialog.exec() != QDialog::Accepted)
        return false;
    return sauvegarderFichier(dialog.selectedFiles().first());
}
//! [12]

//! [13]
void Editeur::about()
//! [13] //! [14]
{
   QMessageBox::about(this, tr("About Application"),
            tr("The <b>Application</b> example demonstrates how to "
               "write modern GUI applications using Qt, with a menu bar, "
               "toolbars, and a status bar."));
}
//! [14]

//! [15]
void Editeur::documentWasModified()
//! [15] //! [16]
{
    setWindowModified(champTexte->document()->isModified());
}
//! [16]

void Editeur::activateFunMode()
{
    QPalette p = champTexte->palette();
    p.setColor(QPalette::Base, Qt::red);
    p.setColor(QPalette::Text, Qt::white);
    champTexte->setPalette(p);
}
//! [17]
void Editeur::creerActions()
//! [17] //! [18]
{

    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    QToolBar *fileToolBar = addToolBar(tr("File"));
    const QIcon newIcon = QIcon::fromTheme("document-new", QIcon(":/images/new.png"));
    QAction *newAct = new QAction(newIcon, tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, &QAction::triggered, this, &Editeur::creer);
    fileMenu->addAction(newAct);
    fileToolBar->addAction(newAct);

//! [19]
    const QIcon openIcon = QIcon::fromTheme("document-open", QIcon(":/images/open.png"));
    QAction *openAct = new QAction(openIcon, tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, &QAction::triggered, this, &Editeur::ouvrir);
    fileMenu->addAction(openAct);
    fileToolBar->addAction(openAct);
//! [18] //! [19]

    const QIcon saveIcon = QIcon::fromTheme("document-save", QIcon(":/images/save.png"));
    QAction *saveAct = new QAction(saveIcon, tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAct, &QAction::triggered, this, &Editeur::sauvegarder);
    fileMenu->addAction(saveAct);
    fileToolBar->addAction(saveAct);

    const QIcon saveAsIcon = QIcon::fromTheme("document-save-as");
    QAction *saveAsAct = fileMenu->addAction(saveAsIcon, tr("Save &As..."), this, &Editeur::sauvegarderSous);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Save the document under a new name"));

//! [20]

    fileMenu->addSeparator();

    const QIcon exitIcon = QIcon::fromTheme("application-exit");
    QAction *exitAct = fileMenu->addAction(exitIcon, tr("E&xit"), this, &QWidget::close);
    exitAct->setShortcuts(QKeySequence::Quit);
//! [20]
    exitAct->setStatusTip(tr("Exit the application"));

//! [21]
    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
    QToolBar *editToolBar = addToolBar(tr("Edit"));
//!
#ifndef QT_NO_CLIPBOARD
    const QIcon cutIcon = QIcon::fromTheme("edit-cut", QIcon(":/images/cut.png"));
    QAction *cutAct = new QAction(cutIcon, tr("Cu&t"), this);
//! [21]
    cutAct->setShortcuts(QKeySequence::Cut);
    cutAct->setStatusTip(tr("Cut the current selection's contents to the "
                            "clipboard"));
    connect(cutAct, &QAction::triggered, champTexte, &QPlainTextEdit::cut);
    editMenu->addAction(cutAct);
    editToolBar->addAction(cutAct);

    const QIcon copyIcon = QIcon::fromTheme("edit-copy", QIcon(":/images/copy.png"));
    QAction *copyAct = new QAction(copyIcon, tr("&Copy"), this);
    copyAct->setShortcuts(QKeySequence::Copy);
    copyAct->setStatusTip(tr("Copy the current selection's contents to the "
                             "clipboard"));
    connect(copyAct, &QAction::triggered, champTexte, &QPlainTextEdit::copy);
    editMenu->addAction(copyAct);
    editToolBar->addAction(copyAct);

    const QIcon pasteIcon = QIcon::fromTheme("edit-paste", QIcon(":/images/paste.png"));
    QAction *pasteAct = new QAction(pasteIcon, tr("&Paste"), this);
    pasteAct->setShortcuts(QKeySequence::Paste);
    pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
                              "selection"));
    connect(pasteAct, &QAction::triggered, champTexte, &QPlainTextEdit::paste);
    editMenu->addAction(pasteAct);
    editToolBar->addAction(pasteAct);

    const QIcon selectAllIcon = QIcon::fromTheme("edit-selectall", QIcon(":/images/selectall.png"));
    QAction *selectAct = new QAction(selectAllIcon, tr("&SelectAll"), this);
    selectAct->setShortcuts(QKeySequence::SelectAll);
    selectAct->setStatusTip(tr("Select the entire "
                              "selection"));
    connect(selectAct, &QAction::triggered, champTexte, &QPlainTextEdit::selectAll);
    editMenu->addAction(selectAct);
    editToolBar->addAction(selectAct);

    const QIcon funModeIcon = QIcon::fromTheme("edit-funMode", QIcon(":/images/funmode.png"));
    QAction *funModeAct = new QAction(funModeIcon, tr("&F"), this);
    funModeAct->setStatusTip(tr("Fun mode "
                              "selection"));
    connect(funModeAct, &QAction::triggered, this, &Editeur::activateFunMode);
    editMenu->addAction(funModeAct);
    editToolBar->addAction(funModeAct);

    menuBar()->addSeparator();

#endif // !QT_NO_CLIPBOARD

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    QAction *aboutAct = helpMenu->addAction(tr("&About"), this, &Editeur::about);
    aboutAct->setStatusTip(tr("Show the application's About box"));

//! [22]

    QAction *aboutQtAct = helpMenu->addAction(tr("About &Qt"), qApp, &QApplication::aboutQt);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
//! [22]

//! [23]
#ifndef QT_NO_CLIPBOARD
    cutAct->setEnabled(false);
//! [23] //! [24]
    copyAct->setEnabled(false);
    connect(champTexte, &QPlainTextEdit::copyAvailable, cutAct, &QAction::setEnabled);
    connect(champTexte, &QPlainTextEdit::copyAvailable, copyAct, &QAction::setEnabled);
#endif // !QT_NO_CLIPBOARD
}
//! [24]

//! [32]
void Editeur::creerBarreEtat()
//! [32] //! [33]
{
    statusBar()->showMessage(tr("Ready"));
}
//! [33]

//! [34] //! [35]
void Editeur::lireOptions()
//! [34] //! [36]
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    const QByteArray geometry = settings.value("geometry", QByteArray()).toByteArray();
    if (geometry.isEmpty()) {
        const QRect availableGeometry = QApplication::desktop()->availableGeometry(this);
        resize(availableGeometry.width() / 3, availableGeometry.height() / 2);
        move((availableGeometry.width() - width()) / 2,
             (availableGeometry.height() - height()) / 2);
    } else {
        restoreGeometry(geometry);
    }
}
//! [35] //! [36]


//! [37] //! [38]
void Editeur::ecrireOptions()
//! [37] //! [39]
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    settings.setValue("geometry", saveGeometry());
}
//! [38] //! [39]

//! [40]
bool Editeur::sauvegarderPeutEtre()
//! [40] //! [41]
{
    if (!champTexte->document()->isModified())
        return true;
    const QMessageBox::StandardButton ret
        = QMessageBox::warning(this, tr("Application"),
                               tr("The document has been modified.\n"
                                  "Do you want to save your changes?"),
                               QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    switch (ret) {
    case QMessageBox::Save:
        return sauvegarder();
    case QMessageBox::Cancel:
        return false;
    default:
        break;
    }
    return true;
}
//! [41]

//! [42]
void Editeur::chargerFichier(const QString &fileName)
//! [42] //! [43]
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName), file.errorString()));
        return;
    }

    QTextStream in(&file);
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    champTexte->setPlainText(in.readAll());
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    setFichier(fileName);
    statusBar()->showMessage(tr("File loaded"), 2000);
}
//! [43]

//! [44]
bool Editeur::sauvegarderFichier(const QString &fileName)
//! [44] //! [45]
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName),
                                  file.errorString()));
        return false;
    }

    QTextStream out(&file);
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    out << champTexte->toPlainText();
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    setFichier(fileName);
    statusBar()->showMessage(tr("File saved"), 2000);
    return true;
}
//! [45]

//! [46]
void Editeur::setFichier(const QString &fileName)
//! [46] //! [47]
{
    fichier = fileName;
    champTexte->document()->setModified(false);
    setWindowModified(false);

    QString shownName = fichier;
    if (fichier.isEmpty())
        shownName = "untitled.txt";
    setWindowFilePath(shownName);
}
//! [47]

//! [48]
QString Editeur::epurerNom(const QString &fullFileName)
//! [48] //! [49]
{
    return QFileInfo(fullFileName).fileName();
}
//! [49]
#ifndef QT_NO_SESSIONMANAGER
void Editeur::commitData(QSessionManager &manager)
{
    if (manager.allowsInteraction()) {
        if (!sauvegarderPeutEtre())
            manager.cancel();
    } else {
        // Non-interactive: save without asking
        if (champTexte->document()->isModified())
            sauvegarder();
    }
}
#endif
