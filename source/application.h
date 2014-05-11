/***************************************************************************
 *   copyright       : (C) 2013 by Quentin BRAMAS                          *
 *   http://texiteasy.com                                                  *
 *                                                                         *
 *   This file is part of texiteasy.                                       *
 *                                                                         *
 *   texiteasy is free software: you can redistribute it and/or modify     *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation, either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   texiteasy is distributed in the hope that it will be useful,          *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with texiteasy.  If not, see <http://www.gnu.org/licenses/>.    *
 *                                                                         *
 ***************************************************************************/

#ifndef APPLICATION_H
#define APPLICATION_H

#include <QString>

#include "singleapp/qtsingleapplication.h"

class Application : public QtSingleApplication
{
    Q_OBJECT
public:
    explicit Application(const QString &id, int & argc, char ** argv );

signals:
    void requestOpenFile(QString filename);
public slots:

protected:
#ifdef OS_MAC
    bool event(QEvent *);
public slots:
    void sendRequestFile();
private:
    QString _filename;
#endif

};

#endif // APPLICATION_H
