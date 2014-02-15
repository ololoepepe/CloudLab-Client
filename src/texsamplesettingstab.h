#ifndef TEXSAMPLESETTINGSTAB_H
#define TEXSAMPLESETTINGSTAB_H

class BPasswordWidget;

class QIcon;
class QCheckBox;
class QLineEdit;
class QByteArray;
class QComboBox;
class QHBoxLayout;
class QToolButton;

#include <BAbstractSettingsTab>

#include <QObject>
#include <QVariantMap>
#include <QStringList>
#include <QString>

/*============================================================================
================================ TexsampleSettingsTab ========================
============================================================================*/

class TexsampleSettingsTab : public BAbstractSettingsTab
{
    Q_OBJECT
public:
    explicit TexsampleSettingsTab();
public:
    QString title() const;
    QIcon icon() const;
    bool restoreDefault();
    bool saveSettings();
private:
    QStringList updateHostHistory(const QStringList &history = QStringList());
private slots:
    void removeCurrentHostFromHistory();
    void cmboxHostCurrentIndexChanged(int index);
private:
    QString autoSelectText;
    QHBoxLayout *mhltHost;
      QComboBox *mcmboxHost;
      QToolButton *mtbtnRemoveFromHistory;
    QLineEdit *mledtLogin;
    BPasswordWidget *mpwdwgt;
    QCheckBox *mcboxAutoconnection;
private:
    Q_DISABLE_COPY(TexsampleSettingsTab)
};

#endif // TEXSAMPLESETTINGSTAB_H
