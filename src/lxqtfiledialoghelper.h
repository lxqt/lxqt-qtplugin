#ifndef LXQTFILEDIALOGHELPER_H
#define LXQTFILEDIALOGHELPER_H

#include <qpa/qplatformdialoghelper.h> // this private header is subject to changes
#include <memory>

namespace Fm {
class FileDialog;
}

class Q_GUI_EXPORT LXQtFileDialogHelper : public QPlatformFileDialogHelper {
    Q_OBJECT

public:
    LXQtFileDialogHelper();

    virtual ~LXQtFileDialogHelper();

    // QPlatformDialogHelper
    void exec() override;
    bool show(Qt::WindowFlags windowFlags, Qt::WindowModality windowModality, QWindow *parent) override;
    void hide() override;

    // QPlatformFileDialogHelper
    bool defaultNameFilterDisables() const override;
    void setDirectory(const QUrl &directory) override;
    QUrl directory() const override;
    void selectFile(const QUrl &filename) override;
    QList<QUrl> selectedFiles() const override;
    void setFilter() override;
    void selectMimeTypeFilter(const QString &filter) override;
    void selectNameFilter(const QString &filter) override;
    QString selectedMimeTypeFilter() const override;
    QString selectedNameFilter() const override;

    bool isSupportedUrl(const QUrl &url) const override;

private:
    void applyOptions();

private:
    std::unique_ptr<Fm::FileDialog> dlg_;
};


#endif // LXQTFILEDIALOGHELPER_H
