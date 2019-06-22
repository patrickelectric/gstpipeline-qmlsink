#include <QDebug>
#include <QStandardPaths>
#include <QString>

/**
 * @brief A simple singleton helper class
 *
 */
class Helper {
public:

    /**
     * @brief Return singleton pointer
     *
     * @return Helper*
     */
    static Helper* self() {
        static Helper helper;
        return &helper;
    }

    /**
     * @brief Return the pathfor the system temporary folder
     *
     * @return QString
     */
    QString temporaryPath() const {
        return _temporaryPath;
    }

private:
    /**
     * @brief Construct a new Helper object
     */
    Helper()
    : _temporaryPath(QStandardPaths::writableLocation(QStandardPaths::TempLocation))
    {
        qDebug() << "Using temporary folder:" << _temporaryPath;
    }

    QString _temporaryPath;
};