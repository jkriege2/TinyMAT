/*
    Copyright (c) 2008-2020 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.


*/

#define _USE_MATH_DEFINES
#include <iostream>
#include <stdio.h>
#include "tinymatwriter.h"
#include <QVariant>
#include <QList>

using namespace std;


int main( int /*argc*/, const char* /*argv*/[] ) {
    TinyMATWriterFile* mat=TinyMATWriter_open("test_qt.mat");
	if (mat) {
        QVariantList varlist;
        varlist<<QString("blabla");
        varlist<<QString("blabla2");
        varlist<<double(1.2);
        varlist<<double(2.2);
        varlist<<double(3.2);
        QList<QList<QVariant> > varmat;
        {
            QList<QVariant> tmp;
            tmp<<1.1<<1.2<<1.3<<"blub";
            varmat<<tmp;
        }
        {
            QList<QVariant> tmp;
            tmp<<2.1<<2.2<<2.3<<2.4;
            varmat<<tmp;
        }
        {
            QList<QVariant> tmp;
            tmp<<3.1<<"blabla"<<3.3<<"blub";
            varmat<<tmp;
        }
        {
            QList<QVariant> tmp;
            tmp<<"blub"<<"blub"<<"blub"<<"blubbbbbbb";
            tmp.append(QVariant(tmp));
            varmat<<tmp;
        }
        QVariantMap smap;
        smap["x"]=1.23;
        smap["y"]=4.56;
        smap["z"]=43;
        smap["str"]="hello World!";
        //smap["varlist"]=varlist;
        smap["smap"]=smap;
        TinyMATWriter_writeString(mat, "string1", "teststring1data blabla");
        TinyMATWriter_writeQVariantList(mat, "vlist1", varlist);
        TinyMATWriter_writeQVariantMatrix_listofcols(mat, "vmatrix1", varmat);
        TinyMATWriter_writeQVariantMap(mat, "vmap1", smap);
        TinyMATWriter_close(mat);
	}
    return 0;
}
