/*
 * Copyright 2012  Daniel Nicoletti <dantti12@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 1.1
import org.kde.plasma.core 0.1 as PlasmaCore
import org.kde.plasma.components 0.1 as PlasmaComponents
import org.kde.apper 0.1 as Apper
import org.packagekit 0.1 as PackageKit

Item {
    id: transaction

    anchors.fill: parent
    clip: true

    property int progressWidth: 30
    signal finished();

    function update(updates) {
        updateTransaction.updatePackages(updates);
    }

    Component.onCompleted: {
        updateTransaction.finished.connect(finished);
    }

    Apper.PkTransaction {
        id: updateTransaction
        onChanged: {
            statusText.text = PkStrings.status(status, speed, downloadSizeRemaining);
            transactionProgress.value = updateTransaction.percentage;
        }
    }

    Column {
        id: actionRow
        spacing: 4
        anchors.margins: 2
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.right: parent.right
        Row {
            spacing: 4
            anchors.margins: 2
            anchors.left: parent.left
            anchors.right: parent.right
            Text {
                id: statusText
                height: parent.height
                width: parent.width - updateBT.width - parent.spacing
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            PlasmaComponents.ToolButton {
                id: updateBT
                iconSource: "dialog-cancel"
                text:  i18n("Cancel")
                enabled: updateTransaction.allowCancel
                onClicked: updateTransaction.cancel();
            }
        }
        PlasmaComponents.ProgressBar {
            id: transactionProgress
            anchors.left: parent.left
            anchors.right: parent.right
        }
    }

    ListView {
        id: progressView
        clip: true
        height: parent.height - actionRow.height - 4
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.margins: 4
        delegate: TransactionProgressDelegate {
        }
        boundsBehavior: Flickable.StopAtBounds
        currentIndex: -1
        model: updateTransaction.progressModel()
    }
}
