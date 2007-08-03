/*
 * This file is part of the KDE project
 *  Copyright (c) 2000 Matthias Elter <elter@kde.org>
 *                2001 John Califf
 *                2004 Boudewijn Rempt <boud@valdyas.org>
 *  Copyright (C) 2007 Thomas Zander <zander@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KoLcmsColorProfile.h"

#include <cfloat>
#include <cmath>
#include <lcms.h>

#include <QImage>
#include <QFile>

#include <kdebug.h>

class KoLcmsColorProfile::Private {
public:
    Private() : valid(false), suitableForOutput(false) { }

    cmsHPROFILE profile;
    icColorSpaceSignature colorSpaceSignature;
    icProfileClassSignature deviceClass;
    QString productDescription;
    QString productInfo;
    QString manufacturer;

    bool valid;
    bool suitableForOutput;
};

KoLcmsColorProfile::KoLcmsColorProfile()
    : d(new Private())
{
}

KoLcmsColorProfile::KoLcmsColorProfile(const QByteArray& rawData)
    : KoIccColorProfile(rawData), d(new Private())
{
    d->profile = cmsOpenProfileFromMem((void*)rawData.constData(), (DWORD)rawData.size());
    init();
}

KoLcmsColorProfile::KoLcmsColorProfile(const QString& file)
    : KoIccColorProfile(file), d(new Private())
{
}

KoLcmsColorProfile::KoLcmsColorProfile(const cmsHPROFILE profile)
    : d(new Private())
{
    setProfile(profile);
}

KoLcmsColorProfile::~KoLcmsColorProfile()
{
    cmsCloseProfile(d->profile);
    delete d;
}

void KoLcmsColorProfile::setProfile(const cmsHPROFILE profile)
{
    d->profile = profile;
    d->valid = true;

    size_t  bytesNeeded=0;

    // Make a raw data image ready for saving
    _cmsSaveProfileToMem(d->profile, 0, &bytesNeeded); // calc size
    QByteArray rawData;
    rawData.resize(bytesNeeded);
    if(rawData.size() >= (int)bytesNeeded)
    {
        _cmsSaveProfileToMem(d->profile, rawData.data(), &bytesNeeded); // fill buffer
        cmsHPROFILE newprofile = cmsOpenProfileFromMem((void*)rawData.constData(), (DWORD) bytesNeeded);
        cmsCloseProfile(d->profile);
        d->profile = newprofile;
    }
    else
        rawData.resize(0);
    setRawData(rawData);
    init();
}

bool KoLcmsColorProfile::load()
{
    QFile file(fileName());
    file.open(QIODevice::ReadOnly);
    QByteArray rawData = file.readAll();
    setRawData(rawData);
    d->profile = cmsOpenProfileFromMem((void*)rawData.constData(), (DWORD)rawData.size());
    file.close();

    if (d->profile == 0) {
        kWarning() << "Failed to load profile from " << fileName();
    }

    return init();

}

bool KoLcmsColorProfile::init()
{
    if (d->profile) {
        d->colorSpaceSignature = cmsGetColorSpace(d->profile);
        d->deviceClass = cmsGetDeviceClass(d->profile);
        d->productDescription = cmsTakeProductDesc(d->profile);
        d->productInfo = cmsTakeProductInfo(d->profile);
        d->valid = true;
        setName(cmsTakeProductName(d->profile));
        setInfo(d->productInfo);

        // Check if the profile can convert (something->this)
//         LPMATSHAPER OutMatShaper = cmsBuildOutputMatrixShaper(d->profile);
//         if( OutMatShaper )
//         {
//             d->suitableForOutput = true;
//         }
        cmsCIEXYZTRIPLE Primaries;

        if (cmsTakeColorants(&Primaries, d->profile))
        {
            d->suitableForOutput = true;
        }

#if 0
    // XXX: It wasn't that easy to save a little memory: thsi gives an lcms error
        // Okay, we know enough. Free the memory; we'll load it again if needed.

        cmsCloseProfile(d->profile);
        d->profile = 0;

#endif
        return true;
    }
    return false;
}

cmsHPROFILE KoLcmsColorProfile::lcmsProfile()
{
#if 0
	if (d->profile = 0) {
	    QFile file(d->filename);
	    file.open(QIODevice::ReadOnly);
	    d->rawData = file.readAll();
	    d->profile = cmsOpenProfileFromMem((void*)d->rawData.constData(), (DWORD)d->rawData.size());
        file.close();
	}
#endif
	return d->profile;
}

bool KoLcmsColorProfile::save()
{
    return false;
}

icColorSpaceSignature KoLcmsColorProfile::colorSpaceSignature() const {
    return d->colorSpaceSignature;
}

icProfileClassSignature KoLcmsColorProfile::deviceClass() const {
    return d->deviceClass;
}

QString KoLcmsColorProfile::productDescription() const {
    return d->productDescription;
}

QString KoLcmsColorProfile::productInfo() const {
    return d->productInfo;
}

QString KoLcmsColorProfile::manufacturer() const {
    return d->manufacturer;
}

bool KoLcmsColorProfile::valid() const {
    return d->valid;
}

bool KoLcmsColorProfile::isSuitableForOutput() const {
    return d->suitableForOutput;
}

bool KoLcmsColorProfile::isSuitableForPrinting() const {
    return deviceClass() == icSigOutputClass;
}

bool KoLcmsColorProfile::isSuitableForDisplay() const {
    return deviceClass() == icSigDisplayClass;
}

bool operator==( const KoLcmsColorProfile & p1,  const KoLcmsColorProfile & p2 )
{
    return p1.d->profile == p2.d->profile;
}

