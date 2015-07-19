/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate: 2015-07-07 12:07:58 +0200 (Di, 07 Jul 2015) $  (revision $Rev: 4005 $)

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


#include <iostream>
#include <stdio.h>
#include "../tinymatwriter.h"
#include <QVariant>
#include <QList>

using namespace std;


int main( int /*argc*/, const char* /*argv*/[] ) {
    TinyMATWriterFile* mat=TinyMATWriter_open("test.mat");
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
            varmat<<tmp;
        }
        TinyMATWriter_writeString(mat, "string1", "teststring1data blabla");
        TinyMATWriter_writeQVariantList(mat, "vlist1", varlist);
        TinyMATWriter_writeQVariantMatrix(mat, "vmatrix1", varmat);
        TinyMATWriter_close(mat);
	}
    return 0;
}
