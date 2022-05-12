/*
 * SPDX-FileCopyrightText: 2018 Nicolas Fella <nicolas.fella@gmx.de>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include <QApplication>
#include <QCommandLineParser>
#include <QStyle>
#include <QStandardPaths>

#include <KCMultiDialog>
#include <KAboutData>
#include <KLocalizedString>
#include <KDBusService>
#include <KWindowSystem>
#include "kdeconnect-version.h"

int main(int argc, char** argv)
{
    QIcon::setFallbackThemeName(QStringLiteral("breeze"));
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication app(argc, argv);
    app.setWindowIcon(QIcon::fromTheme(QStringLiteral("kdeconnect")));
    KAboutData about(QStringLiteral("kdeconnect-settings"),
                     i18n("KDE Connect Settings"),
                     QStringLiteral(KDECONNECT_VERSION_STRING),
                     i18n("KDE Connect Settings"),
                     KAboutLicense::GPL,
                     i18n("(C) 2018-2020 Nicolas Fella"));
    KAboutData::setApplicationData(about);

    QCommandLineParser parser;
    parser.addOption(QCommandLineOption(QStringLiteral("args"), i18n("Arguments for the config module"), QStringLiteral("args")));

    about.setupCommandLine(&parser);
    parser.process(app);
    about.processCommandLine(&parser);

    KDBusService dbusService(KDBusService::Unique);

    KCMultiDialog* dialog = new KCMultiDialog;
    dialog->addModule(KPluginMetaData(QStringLiteral("kcm_kdeconnect")), {parser.value(QStringLiteral("args"))});

    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->show();

    QObject::connect(&dbusService, &KDBusService::activateRequested, dialog, [dialog](const QStringList &args, const QString &/*workingDir*/) {
        KWindowSystem::updateStartupId(dialog->windowHandle());
        KWindowSystem::activateWindow(dialog->windowHandle());

        QCommandLineParser parser;
        parser.addOption(QCommandLineOption(QStringLiteral("args"), i18n("Arguments for the config module"), QStringLiteral("args")));
        parser.parse(args);

        dialog->clear();
        dialog->addModule(KPluginMetaData(QStringLiteral("kcm_kdeconnect")), {parser.value(QStringLiteral("args"))});
    });

    app.setQuitOnLastWindowClosed(true);

    return app.exec();
}

