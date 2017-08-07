#include "lxqtfiledialoghelper.h"

#include <libfm-qt/libfmqt.h>
#include <libfm-qt/filedialog.h>

#include <QWindow>
#include <QMimeDatabase>
#include <QDebug>
#include <QTimer>

#include <memory>

static std::unique_ptr<Fm::LibFmQt> libfmQtContext_;

LXQtFileDialogHelper::LXQtFileDialogHelper() {
    if(!libfmQtContext_) {
        // initialize libfm-qt only once
        libfmQtContext_ = std::unique_ptr<Fm::LibFmQt>{new Fm::LibFmQt()};
    }

    // can only be used after libfm-qt initialization
    dlg_ = std::unique_ptr<Fm::FileDialog>(new Fm::FileDialog());
    connect(dlg_.get(), &Fm::FileDialog::accepted, this, &LXQtFileDialogHelper::accept);
    connect(dlg_.get(), &Fm::FileDialog::rejected, this, &LXQtFileDialogHelper::reject);

    connect(dlg_.get(), &Fm::FileDialog::fileSelected, this, &LXQtFileDialogHelper::fileSelected);
    connect(dlg_.get(), &Fm::FileDialog::filesSelected, this, &LXQtFileDialogHelper::filesSelected);
    connect(dlg_.get(), &Fm::FileDialog::currentChanged, this, &LXQtFileDialogHelper::currentChanged);
    connect(dlg_.get(), &Fm::FileDialog::directoryEntered, this, &LXQtFileDialogHelper::directoryEntered);
    connect(dlg_.get(), &Fm::FileDialog::filterSelected, this, &LXQtFileDialogHelper::filterSelected);
}

LXQtFileDialogHelper::~LXQtFileDialogHelper() {
}

void LXQtFileDialogHelper::exec() {
    dlg_->exec();
}

bool LXQtFileDialogHelper::show(Qt::WindowFlags windowFlags, Qt::WindowModality windowModality, QWindow* parent) {
    dlg_->setAttribute(Qt::WA_NativeWindow, true); // without this, sometimes windowHandle() will return nullptr

    dlg_->setWindowFlags(windowFlags);
    dlg_->setWindowModality(windowModality);

    // Reference: KDE implementation
    // https://github.com/KDE/plasma-integration/blob/master/src/platformtheme/kdeplatformfiledialoghelper.cpp
    dlg_->windowHandle()->setTransientParent(parent);

    applyOptions();

    // NOTE: the timer here is required as a workaround borrowed from KDE. Without this, the dialog UI will be blocked.
    // QFileDialog calls our platform plugin to show our own native file dialog instead of showing its widget.
    // However, it still creates a hidden dialog internally, and then make it modal.
    // So user input from all other windows that are not the children of the QFileDialog widget will be blocked.
    // This includes our own dialog. After the return of this show() method, QFileDialog creates its own window and
    // then make it modal, which blocks our UI. The timer schedule a delayed popup of our file dialog, so we can
    // show again after QFileDialog and override the modal state. Then our UI can be unblocked.
    QTimer::singleShot(0, dlg_.get(), &QDialog::show);
    dlg_->setFocus();
    return true;
}

void LXQtFileDialogHelper::hide() {
    dlg_->hide();
}

bool LXQtFileDialogHelper::defaultNameFilterDisables() const {
    return false;
}

void LXQtFileDialogHelper::setDirectory(const QUrl& directory) {
    dlg_->setDirectory(directory);
}

QUrl LXQtFileDialogHelper::directory() const {
    return dlg_->directory();
}

void LXQtFileDialogHelper::selectFile(const QUrl& filename) {
    dlg_->selectFile(filename);
}

QList<QUrl> LXQtFileDialogHelper::selectedFiles() const {
    return dlg_->selectedFiles();
}

void LXQtFileDialogHelper::setFilter() {
    // FIXME: what's this?
    // The gtk+ 3 file dialog helper in Qt5 update options in this method.
    applyOptions();
}

void LXQtFileDialogHelper::selectNameFilter(const QString& filter) {
    dlg_->selectNameFilter(filter);
}

#if QT_VERSION >= QT_VERSION_CHECK(5, 9, 0)
QString LXQtFileDialogHelper::selectedMimeTypeFilter() const {
    const auto mimeTypeFromFilter = QMimeDatabase().mimeTypeForName(dlg_->selectedNameFilter());
    if(mimeTypeFromFilter.isValid()) {
        return mimeTypeFromFilter.name();
    }
    QList<QUrl> sel = dlg_->selectedFiles();
    if(sel.isEmpty()) {
        return QString();
    }
    return QMimeDatabase().mimeTypeForUrl(sel.at(0)).name();
}

void LXQtFileDialogHelper::selectMimeTypeFilter(const QString& filter) {
    dlg_->selectNameFilter(filter);
}
#endif

QString LXQtFileDialogHelper::selectedNameFilter() const {
    return dlg_->selectedNameFilter();
}

bool LXQtFileDialogHelper::isSupportedUrl(const QUrl& url) const {
    return dlg_->isSupportedUrl(url);
}

void LXQtFileDialogHelper::applyOptions() {
    auto& opt = options();
    if(options()->testOption(QFileDialogOptions::ShowDirsOnly)) {
        if(!options()->windowTitle().isEmpty()) {
            dlg_->setWindowTitle(options()->windowTitle());
        }
    }
    else {
        if(options()->windowTitle().isEmpty()) {
            dlg_->setWindowTitle(options()->acceptMode() == QFileDialogOptions::AcceptOpen ? tr("Open File")
                                                                                           : tr("Save File"));
        }
        else {
            dlg_->setWindowTitle(options()->windowTitle());
        }
    }
    dlg_->setFilter(opt->filter());
    dlg_->setViewMode(opt->viewMode() == QFileDialogOptions::Detail ? Fm::FolderView::DetailedListMode : Fm::FolderView::CompactMode);
    dlg_->setFileMode(QFileDialog::FileMode(opt->fileMode()));
    dlg_->setAcceptMode(QFileDialog::AcceptMode(opt->acceptMode()));
    // bool useDefaultNameFilters() const;
    dlg_->setNameFilters(opt->nameFilters());
    if(!opt->mimeTypeFilters().empty()) {
        dlg_->setMimeTypeFilters(opt->mimeTypeFilters());
    }

    dlg_->setDefaultSuffix(opt->defaultSuffix());
    // QStringList history() const;

    for(int i = 0; i < QFileDialogOptions::DialogLabelCount; ++i) {
        auto label = static_cast<QFileDialogOptions::DialogLabel>(i);
        if(opt->isLabelExplicitlySet(label)) {
            dlg_->setLabelText(static_cast<QFileDialog::DialogLabel>(label), opt->labelText(label));
        }
    }

    auto url = opt->initialDirectory();
    if(url.isValid()) {
        dlg_->setDirectory(url);
    }


#if QT_VERSION >= QT_VERSION_CHECK(5, 9, 0)
    auto filter = opt->initiallySelectedMimeTypeFilter();
    if(!filter.isEmpty()) {
        selectMimeTypeFilter(filter);
    }
    else {
        filter = opt->initiallySelectedNameFilter();
        if(!filter.isEmpty()) {
            selectNameFilter(options()->initiallySelectedNameFilter());
        }
    }
#else
    filter = opt->initiallySelectedNameFilter();
    if(!filter.isEmpty()) {
        selectNameFilter(filter);
    }
#endif

    auto selectedFiles = opt->initiallySelectedFiles();
    for(const auto& selectedFile: selectedFiles) {
        selectFile(selectedFile);
    }
    // QStringList supportedSchemes() const;
}

/*
FileDialogPlugin::FileDialogPlugin() {

}

QPlatformFileDialogHelper *FileDialogPlugin::createHelper() {
    return new LXQtFileDialogHelper();
}
*/
