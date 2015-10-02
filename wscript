def options(context):
    context.load("compiler_cxx gnu_dirs qt5")


def configure(context):
    context.load("compiler_cxx gnu_dirs qt5")
    context.check_cfg(package="nemonotifications-qt5", args="--libs --cflags")
    context.check_cfg(package="sailfishapp", args="--libs --cflags")


def build(context):
    context.program(
        target="harbour-tremotesf",
        features="qt5",
        includes="src src/folderlistmodel",
        uselib="NEMONOTIFICATIONS-QT5 QT5DBUS QT5QUICK SAILFISHAPP",
        source=[
            "src/accountmodel.cpp",
            "src/appsettings.cpp",
            "src/dbusproxy.cpp",
            "src/notifications.cpp",
            "src/proxytorrentmodel.cpp",
            "src/transmission.cpp",
            "src/tremotesf.cpp",
            "src/torrentmodel.cpp",
            "src/torrentfilemodel.cpp",
            "src/torrentpeermodel.cpp",
            "src/torrenttrackermodel.cpp",
            "src/folderlistmodel/fileinfothread.cpp",
            "src/folderlistmodel/qquickfolderlistmodel.cpp"
        ],
        linkflags="-pie -rdynamic",
        lang=[
            "translations/harbour-tremotesf-en",
            "translations/harbour-tremotesf-ru"
        ]
    )

    context.install_files("${DATADIR}/harbour-tremotesf",
                          context.path.ant_glob("qml/**/*.qml"),
                          relative_trick=True)

    context.install_files("${DATADIR}/harbour-tremotesf", "cover.png")

    context.install_files("${DATADIR}/harbour-tremotesf/translations",
                          context.path.get_bld().ant_glob("translations/*.qm"))

    context.install_files("${DATADIR}/applications",
                          "harbour-tremotesf.desktop")

    context.install_files("${DATADIR}/icons/hicolor/86x86/apps",
                          "harbour-tremotesf.png")
