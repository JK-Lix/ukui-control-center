#ifndef WIDGET_H
#define WIDGET_H

#include <QMutex>
#include <QWidget>
#include <QVariantMap>
#include <QGSettings>
#include <QSettings>
#include <QButtonGroup>
#include <QDBusInterface>
#include <QDBusConnection>
#include <QDBusError>
#include <QDBusReply>
#include <QStringList>
#include <QSharedPointer>
#include <QHash>
#include <QVariant>
#include <QScreen>
#include <QMultiMap>
#include <QShortcut>
#include <QFuture>

#include <KF5/KScreen/kscreen/config.h>
#include <unistd.h>

#include "outputconfig.h"
#include "SwitchButton/switchbutton.h"
#include "brightnessFrame.h"
#include "screenConfig.h"

class QLabel;
class QMLOutput;
class QMLScreen;
class ControlPanel;
class PrimaryOutputCombo;

class QPushButton;
class QComboBox;

class QQuickView;
class QQuickWidget;
class QStyledItemDelegate;

typedef enum {
    SUN,
    CUSTOM,
}MODE;

namespace KScreen {
class ConfigOperation;
}

namespace Ui {
class DisplayWindow;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget() override;

    void setConfig(const KScreen::ConfigPtr &config, bool showBrightnessFrameFlag = false);
    KScreen::ConfigPtr currentConfig() const;

    void slotFocusedOutputChangedNoParam();
    void initConnection();
    QString getScreenName(QString name = "");
    void initTemptSlider();

    bool writeFile(const QString &filePath);
    void writeGlobal(const KScreen::OutputPtr &output);
    bool writeGlobalPart(const KScreen::OutputPtr &output, QVariantMap &info,
                         const KScreen::OutputPtr &fallback);
    QString globalFileName(const QString &hash);
    QVariantMap getGlobalData(KScreen::OutputPtr output);

    float converToScale(const int value);
    int scaleToSlider(const float value);

    void initUiComponent();
    void addBrightnessFrame(QString name, bool openFlag);
    void showBrightnessFrame(const int flag = 0);
protected:
    bool eventFilter(QObject *object, QEvent *event) override;

Q_SIGNALS:
    void changed();

    void nightModeChanged(const bool nightMode) const;
    void redShiftValidChanged(const bool isValid) const;

private Q_SLOTS:
    void slotFocusedOutputChanged(QMLOutput *output);

    void slotOutputEnabledChanged();
    void slotOutputConnectedChanged();
    void slotQmloutOutChanged();

    void slotUnifyOutputs();

    void slotIdentifyButtonClicked(bool checked = true);
    void slotIdentifyOutputs(KScreen::ConfigOperation *op);
    void clearOutputIdentifiers();

    void outputAdded(const KScreen::OutputPtr &output);
    void outputRemoved(int outputId);
    void primaryOutputSelected(int index);
    void primaryOutputChanged(const KScreen::OutputPtr &output);

    void showNightWidget(bool judge);
    void showCustomWiget(int index);

    void slotThemeChanged(bool judge);

    void primaryButtonEnable(bool);             // 按钮选择主屏确认按钮
    void mainScreenButtonSelect(int index);     // 是否禁用设置主屏按钮
    void checkOutputScreen(bool judge);         // 是否禁用屏幕

    void setNightMode(const bool nightMode);    // 设置夜间模式

    void initNightStatus();                     // 初始化夜间模式
    void nightChangedSlot(QHash<QString, QVariant> nightArg);

    void callMethod(QRect geometry, QString name);// 设置wayland主屏幕
    QString getPrimaryWaylandScreen();
    void isWayland();

    void kdsScreenchangeSlot(QString status);

public Q_SLOTS:
    void save();
    void scaleChangedSlot(double scale);
    void changedSlot();
    void propertiesChangedSlot(QString, QMap<QString, QVariant>, QStringList);

private:
    void loadQml();
    void resetPrimaryCombo();
    void addOutputToPrimaryCombo(const KScreen::OutputPtr &output);
    KScreen::OutputPtr findOutput(const KScreen::ConfigPtr &config, const QVariantMap &info);

    void setHideModuleInfo();
    void setTitleLabel();
    void writeScale(double scale);
    void initGSettings();
    void initNightUI();

    bool isRestoreConfig();                       // 是否恢复应用之前的配置
    bool isCloneMode();
    bool isBacklight();
    bool isLaptopScreen();
    bool isVisibleBrightness();

    QString getCpuInfo();
    QString getMonitorType();

    int getDDCBrighthess();
    int getDDCBrighthess(QString name);
    int getLaptopBrightness() const;
    int getPrimaryScreenID();

    void setDDCBrightnessN(int value, QString screenName);
    void setScreenKDS(QString kdsConfig);
    void setActiveScreen(QString status = "");

    QList<ScreenConfig> getPreScreenCfg();
    void setPreScreenCfg(KScreen::OutputList screens);

private:
    Ui::DisplayWindow *ui;
    QMLScreen *mScreen = nullptr;

#if QT_VERSION <= QT_VERSION_CHECK(5, 12, 0)
    KScreen::ConfigPtr mConfig;
    KScreen::ConfigPtr mPrevConfig;
    KScreen::OutputPtr res;                       // 这是outPutptr结果
#else
    KScreen::ConfigPtr mConfig          = nullptr;
    KScreen::ConfigPtr mPrevConfig      = nullptr;
    KScreen::ConfigPtr mPreScreenConfig = nullptr;
    KScreen::OutputPtr res              = nullptr;
#endif

    ControlPanel *mControlPanel = nullptr;

    OutputConfig *mOutputConfig = nullptr;        // 设置主显示器相关控件

    QList<QQuickView *> mOutputIdentifiers;
    QTimer *mOutputTimer = nullptr;

    QMutex      mLock;

    QString     mCPU;
    QString     mDir;
    QStringList mPowerKeys;
    QString     mKDSCfg;

    SwitchButton *mNightButton = nullptr;
    SwitchButton *mCloseScreenButton = nullptr;
    SwitchButton *mUnifyButton = nullptr;
    SwitchButton *mThemeButton = nullptr;

    QLabel *nightLabel = nullptr;

    QGSettings *mGsettings = nullptr;
    QGSettings *scaleGSettings = nullptr;
    QGSettings *mPowerGSettings = nullptr;

    QSettings *mQsettings = nullptr;

    QButtonGroup *singleButton;

    QSharedPointer<QDBusInterface> mUPowerInterface;
    QSharedPointer<QDBusInterface> mUkccInterface;

    QHash<QString, QVariant> mNightConfig;

    double mScreenScale = 1.0;
    int mScreenId = -1;

    bool mIsNightMode     = false;
    bool mRedshiftIsValid = false;
    bool mIsScaleChanged  = false;
    bool mOriApply;
    bool mConfigChanged   = false;
    bool mOnBattery       = false;
    bool mIsUnifyChanged  = false;
    bool mFirstLoad       = true;
    bool mIsWayland       = false;
    bool mIsBattery       = false;

    bool threadRunExit = false;
    QFuture<void> threadRun;
    
    QShortcut *mApplyShortcut;
    QVector<BrightnessFrame*> BrightnessFrameV;
    QVector<QString> deleteFrameNameV;  //用二级指针判断null出现问题，只想到这种方式排除段错误
    BrightnessFrame *currentBrightnessFrame;
    bool exitFlag = false;

};

#endif // WIDGET_H
