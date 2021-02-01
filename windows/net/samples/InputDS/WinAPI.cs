/*
 *  Copyright (c) 2013 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/
using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;

namespace InputDS
{
    static class WinAPI
    {
        internal const int E_FAIL = unchecked((int)0x80004005); // -2147467259 == 0x80004005L
        internal const int S_OK = 0;
    }
}
