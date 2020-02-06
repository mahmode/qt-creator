/****************************************************************************
**
** Copyright (C) 2020 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
****************************************************************************/

import QtQuick 2.12
import QtQuick3D 1.14

View3D {
    id: sceneView
    anchors.fill: parent

    property bool usePerspective: false
    property bool showSceneLight: false
    property alias sceneHelpers: sceneHelpers
    property alias perpectiveCamera: scenePerspectiveCamera
    property alias orthoCamera: sceneOrthoCamera
    property double cameraZoomFactor;

    // Empirical cameraZoomFactor values that happen every near consistent mouse wheel scrolls
    readonly property var grid_thresholds: [0.1, 0.265, 0.55, 1.10, 2.35, 4.9, 10.0, 20.5, 42.0, 85.0, 999999.0]
    property var thresIdx: 1

    camera: usePerspective ? scenePerspectiveCamera : sceneOrthoCamera

    Node {
        id: sceneHelpers

        HelperGrid {
            id: helperGrid
            lines: 20 + Math.round((1 - subdivAlpha) * 20);
            step: {
                thresIdx = Math.max(1, grid_thresholds.findIndex(v => v > cameraZoomFactor));
                return 100 * grid_thresholds[0] * Math.pow(2, thresIdx - 1);
            }
            subdivAlpha: (grid_thresholds[thresIdx] - cameraZoomFactor) / (grid_thresholds[thresIdx] - grid_thresholds[thresIdx - 1]);
        }

        PointLight {
            id: sceneLight
            visible: showSceneLight
            position: usePerspective ? scenePerspectiveCamera.position
                                     : sceneOrthoCamera.position
            quadraticFade: 0
            linearFade: 0
        }

        // Initial camera position and rotation should be such that they look at origin.
        // Otherwise EditCameraController._lookAtPoint needs to be initialized to correct
        // point.
        PerspectiveCamera {
            id: scenePerspectiveCamera
            z: -600
            y: 600
            rotation.x: 45
            clipFar: 100000
            clipNear: 1
        }

        OrthographicCamera {
            id: sceneOrthoCamera
            z: -600
            y: 600
            rotation.x: 45
            clipFar: 100000
            clipNear: -10000
        }
    }
}
