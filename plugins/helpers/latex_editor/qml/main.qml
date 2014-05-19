/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.1
import QtQuick.Controls 1.0

Rectangle {
    id: page
    width: 440; height: 250
    color: "#e8e8e8"
    GridView {
        id: grid_view
        x: 8
        y: 8
        property int cols: 5
        property int rows: 2
        width: 400
        height: 120
        delegate: Item {
            x: 5
            height: 60
            width:80
            Column {
                spacing: 2

                SearchBox {
                    width: 80
                    height: 60
                }
            }
        }
        model: ListModel {
            id:tableModel
            ListElement {
            }
            ListElement {
            }
            ListElement {
            }
            ListElement {
            }
            ListElement {
            }
            ListElement {
            }
            ListElement {
            }
            ListElement {
            }
            ListElement {
            }
            ListElement {
            }
        }
        cellHeight: 60
        cellWidth: 80
    }


    Button{
        text: "+"
        x: 412
        y: 8
        width: 20
        height: 60
        onClicked: {
            for(var i = 1; i < grid_view.rows + 1; ++i)
            {
                tableModel.insert(grid_view.cols * i, {})
            }
            grid_view.cols += 1
            grid_view.width = 82 * grid_view.cols
            x = grid_view.width + 0
            parent.width = grid_view.width + 28
        }
    }


}
