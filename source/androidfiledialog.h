// Credits:
// https://stackoverflow.com/questions/15079406/qt-necessitas-reasonable-qfiledialog-replacement-skin


#ifndef ANDROIDFILEDIALOG_H
#define ANDROIDFILEDIALOG_H

#include <QObject>

#ifdef Q_OS_ANDROID

#include <QAndroidJniObject>
#include <QtAndroid>
#include <QAndroidActivityResultReceiver>

class AndroidFileDialog : public QObject
{
    Q_OBJECT

public:
    explicit AndroidFileDialog(QObject *parent = 0);
    virtual ~AndroidFileDialog();
    bool provideExistingFileName();

private:
    class ResultReceiver : public QAndroidActivityResultReceiver {
        AndroidFileDialog *_dialog;
    public:
        ResultReceiver(AndroidFileDialog *dialog);
        virtual ~ResultReceiver();
        void handleActivityResult(int receiverRequestCode, int resultCode, const QAndroidJniObject &data);
        QString uriToPath(QAndroidJniObject uri);
    };

    static const int EXISTING_FILE_NAME_REQUEST = 1;
    ResultReceiver *receiver;
    void emitExistingFileNameReady(QString result);

signals:
    void existingFileNameReady(QString result);
};

#endif

#endif // ANDROIDFILEDIALOG_H


