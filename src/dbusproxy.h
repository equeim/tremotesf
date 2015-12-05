/*
 * Tremotesf
 * Copyright (C) 2015 Alexey Rochev <equeim@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TREMOTESF_DBUSPROXY_H
#define TREMOTESF_DBUSPROXY_H

#include <QObject>

class QWindow;

namespace Tremotesf
{

class DBusProxy : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "harbour.tremotesf.DBusProxy")
public:
    DBusProxy(QWindow *window);
public slots:
    void activate();
private:
    QWindow *m_window;
};

}

#endif // TREMOTESF_DBUSPROXY_H
