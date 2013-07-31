/* Everything */

/* Qt Edition */
#ifndef QT_EDITION
#  define QT_EDITION QT_EDITION_OPENSOURCE
#endif

/* Machine byte-order */
#define Q_BIG_ENDIAN 4321
#define Q_LITTLE_ENDIAN 1234
#define QT_BUILD_KEY "arm linux oe-g++ full-config"
#define QT_BUILD_KEY_COMPAT "i686 Linux oe-g++ full-config"

#ifdef QT_BOOTSTRAPPED
#define Q_BYTE_ORDER Q_LITTLE_ENDIAN
#else
#define Q_BYTE_ORDER Q_LITTLE_ENDIAN
#endif
/* Machine Architecture */
#ifndef QT_BOOTSTRAPPED
# define QT_ARCH_ARM
#else
# define QT_ARCH_I386
#endif
/* Compile time features */
#define QT_LARGEFILE_SUPPORT 64
#define QT_POINTER_SIZE 4

#if defined(QT_BUILTIN_GIF_READER) && defined(QT_NO_BUILTIN_GIF_READER)
# undef QT_BUILTIN_GIF_READER
#elif !defined(QT_BUILTIN_GIF_READER) && !defined(QT_NO_BUILTIN_GIF_READER)
# define QT_BUILTIN_GIF_READER 1
#endif

#if defined(QT_NO_ACCESSIBILITY) && defined(QT_ACCESSIBILITY)
# undef QT_NO_ACCESSIBILITY
#elif !defined(QT_NO_ACCESSIBILITY) && !defined(QT_ACCESSIBILITY)
# define QT_NO_ACCESSIBILITY
#endif

#if defined(QT_NO_CUPS) && defined(QT_CUPS)
# undef QT_NO_CUPS
#elif !defined(QT_NO_CUPS) && !defined(QT_CUPS)
# define QT_NO_CUPS
#endif

#if defined(QT_NO_DECLARATIVE) && defined(QT_DECLARATIVE)
# undef QT_NO_DECLARATIVE
#elif !defined(QT_NO_DECLARATIVE) && !defined(QT_DECLARATIVE)
# define QT_NO_DECLARATIVE
#endif

#if defined(QT_NO_EGL) && defined(QT_EGL)
# undef QT_NO_EGL
#elif !defined(QT_NO_EGL) && !defined(QT_EGL)
# define QT_NO_EGL
#endif

#if defined(QT_NO_EXCEPTIONS) && defined(QT_EXCEPTIONS)
# undef QT_NO_EXCEPTIONS
#elif !defined(QT_NO_EXCEPTIONS) && !defined(QT_EXCEPTIONS)
# define QT_NO_EXCEPTIONS
#endif

#if defined(QT_NO_ICONV) && defined(QT_ICONV)
# undef QT_NO_ICONV
#elif !defined(QT_NO_ICONV) && !defined(QT_ICONV)
# define QT_NO_ICONV
#endif

#if defined(QT_NO_IMAGEFORMAT_JPEG) && defined(QT_IMAGEFORMAT_JPEG)
# undef QT_NO_IMAGEFORMAT_JPEG
#elif !defined(QT_NO_IMAGEFORMAT_JPEG) && !defined(QT_IMAGEFORMAT_JPEG)
# define QT_NO_IMAGEFORMAT_JPEG
#endif

#if defined(QT_NO_IMAGEFORMAT_MNG) && defined(QT_IMAGEFORMAT_MNG)
# undef QT_NO_IMAGEFORMAT_MNG
#elif !defined(QT_NO_IMAGEFORMAT_MNG) && !defined(QT_IMAGEFORMAT_MNG)
# define QT_NO_IMAGEFORMAT_MNG
#endif

#if defined(QT_NO_IMAGEFORMAT_TIFF) && defined(QT_IMAGEFORMAT_TIFF)
# undef QT_NO_IMAGEFORMAT_TIFF
#elif !defined(QT_NO_IMAGEFORMAT_TIFF) && !defined(QT_IMAGEFORMAT_TIFF)
# define QT_NO_IMAGEFORMAT_TIFF
#endif

#if defined(QT_NO_NAS) && defined(QT_NAS)
# undef QT_NO_NAS
#elif !defined(QT_NO_NAS) && !defined(QT_NAS)
# define QT_NO_NAS
#endif

#if defined(QT_NO_NIS) && defined(QT_NIS)
# undef QT_NO_NIS
#elif !defined(QT_NO_NIS) && !defined(QT_NIS)
# define QT_NO_NIS
#endif

#if defined(QT_NO_OPENGL) && defined(QT_OPENGL)
# undef QT_NO_OPENGL
#elif !defined(QT_NO_OPENGL) && !defined(QT_OPENGL)
# define QT_NO_OPENGL
#endif

#if defined(QT_NO_OPENVG) && defined(QT_OPENVG)
# undef QT_NO_OPENVG
#elif !defined(QT_NO_OPENVG) && !defined(QT_OPENVG)
# define QT_NO_OPENVG
#endif

#if defined(QT_NO_QWS_DIRECTFB) && defined(QT_QWS_DIRECTFB)
# undef QT_NO_QWS_DIRECTFB
#elif !defined(QT_NO_QWS_DIRECTFB) && !defined(QT_QWS_DIRECTFB)
# define QT_NO_QWS_DIRECTFB
#endif

#if defined(QT_NO_QWS_KBD_LINUXINPUT) && defined(QT_QWS_KBD_LINUXINPUT)
# undef QT_NO_QWS_KBD_LINUXINPUT
#elif !defined(QT_NO_QWS_KBD_LINUXINPUT) && !defined(QT_QWS_KBD_LINUXINPUT)
# define QT_NO_QWS_KBD_LINUXINPUT
#endif

#if defined(QT_NO_QWS_KBD_QNX) && defined(QT_QWS_KBD_QNX)
# undef QT_NO_QWS_KBD_QNX
#elif !defined(QT_NO_QWS_KBD_QNX) && !defined(QT_QWS_KBD_QNX)
# define QT_NO_QWS_KBD_QNX
#endif

#if defined(QT_NO_QWS_KBD_QVFB) && defined(QT_QWS_KBD_QVFB)
# undef QT_NO_QWS_KBD_QVFB
#elif !defined(QT_NO_QWS_KBD_QVFB) && !defined(QT_QWS_KBD_QVFB)
# define QT_NO_QWS_KBD_QVFB
#endif

#if defined(QT_NO_QWS_KBD_UM) && defined(QT_QWS_KBD_UM)
# undef QT_NO_QWS_KBD_UM
#elif !defined(QT_NO_QWS_KBD_UM) && !defined(QT_QWS_KBD_UM)
# define QT_NO_QWS_KBD_UM
#endif

#if defined(QT_NO_QWS_MOUSE_QNX) && defined(QT_QWS_MOUSE_QNX)
# undef QT_NO_QWS_MOUSE_QNX
#elif !defined(QT_NO_QWS_MOUSE_QNX) && !defined(QT_QWS_MOUSE_QNX)
# define QT_NO_QWS_MOUSE_QNX
#endif

#if defined(QT_NO_QWS_MOUSE_TSLIB) && defined(QT_QWS_MOUSE_TSLIB)
# undef QT_NO_QWS_MOUSE_TSLIB
#elif !defined(QT_NO_QWS_MOUSE_TSLIB) && !defined(QT_QWS_MOUSE_TSLIB)
# define QT_NO_QWS_MOUSE_TSLIB
#endif

#if defined(QT_NO_QWS_QNX) && defined(QT_QWS_QNX)
# undef QT_NO_QWS_QNX
#elif !defined(QT_NO_QWS_QNX) && !defined(QT_QWS_QNX)
# define QT_NO_QWS_QNX
#endif

#if defined(QT_NO_QWS_QVFB) && defined(QT_QWS_QVFB)
# undef QT_NO_QWS_QVFB
#elif !defined(QT_NO_QWS_QVFB) && !defined(QT_QWS_QVFB)
# define QT_NO_QWS_QVFB
#endif

#if defined(QT_NO_QWS_TRANSFORMED) && defined(QT_QWS_TRANSFORMED)
# undef QT_NO_QWS_TRANSFORMED
#elif !defined(QT_NO_QWS_TRANSFORMED) && !defined(QT_QWS_TRANSFORMED)
# define QT_NO_QWS_TRANSFORMED
#endif

#if defined(QT_NO_QWS_VNC) && defined(QT_QWS_VNC)
# undef QT_NO_QWS_VNC
#elif !defined(QT_NO_QWS_VNC) && !defined(QT_QWS_VNC)
# define QT_NO_QWS_VNC
#endif

#if defined(QT_NO_SESSIONMANAGER) && defined(QT_SESSIONMANAGER)
# undef QT_NO_SESSIONMANAGER
#elif !defined(QT_NO_SESSIONMANAGER) && !defined(QT_SESSIONMANAGER)
# define QT_NO_SESSIONMANAGER
#endif

#if defined(QT_NO_STYLE_GTK) && defined(QT_STYLE_GTK)
# undef QT_NO_STYLE_GTK
#elif !defined(QT_NO_STYLE_GTK) && !defined(QT_STYLE_GTK)
# define QT_NO_STYLE_GTK
#endif

#if defined(QT_NO_SXE) && defined(QT_SXE)
# undef QT_NO_SXE
#elif !defined(QT_NO_SXE) && !defined(QT_SXE)
# define QT_NO_SXE
#endif

#if defined(QT_NO_XMLPATTERNS) && defined(QT_XMLPATTERNS)
# undef QT_NO_XMLPATTERNS
#elif !defined(QT_NO_XMLPATTERNS) && !defined(QT_XMLPATTERNS)
# define QT_NO_XMLPATTERNS
#endif

#if defined(QT_NO_ZLIB) && defined(QT_ZLIB)
# undef QT_NO_ZLIB
#elif !defined(QT_NO_ZLIB) && !defined(QT_ZLIB)
# define QT_NO_ZLIB
#endif

#if defined(QT_QWS_DEPTH_1) && defined(QT_NO_QWS_DEPTH_1)
# undef QT_QWS_DEPTH_1
#elif !defined(QT_QWS_DEPTH_1) && !defined(QT_NO_QWS_DEPTH_1)
# define QT_QWS_DEPTH_1
#endif

#if defined(QT_QWS_DEPTH_12) && defined(QT_NO_QWS_DEPTH_12)
# undef QT_QWS_DEPTH_12
#elif !defined(QT_QWS_DEPTH_12) && !defined(QT_NO_QWS_DEPTH_12)
# define QT_QWS_DEPTH_12
#endif

#if defined(QT_QWS_DEPTH_15) && defined(QT_NO_QWS_DEPTH_15)
# undef QT_QWS_DEPTH_15
#elif !defined(QT_QWS_DEPTH_15) && !defined(QT_NO_QWS_DEPTH_15)
# define QT_QWS_DEPTH_15
#endif

#if defined(QT_QWS_DEPTH_16) && defined(QT_NO_QWS_DEPTH_16)
# undef QT_QWS_DEPTH_16
#elif !defined(QT_QWS_DEPTH_16) && !defined(QT_NO_QWS_DEPTH_16)
# define QT_QWS_DEPTH_16
#endif

#if defined(QT_QWS_DEPTH_18) && defined(QT_NO_QWS_DEPTH_18)
# undef QT_QWS_DEPTH_18
#elif !defined(QT_QWS_DEPTH_18) && !defined(QT_NO_QWS_DEPTH_18)
# define QT_QWS_DEPTH_18
#endif

#if defined(QT_QWS_DEPTH_2) && defined(QT_NO_QWS_DEPTH_2)
# undef QT_QWS_DEPTH_2
#elif !defined(QT_QWS_DEPTH_2) && !defined(QT_NO_QWS_DEPTH_2)
# define QT_QWS_DEPTH_2
#endif

#if defined(QT_QWS_DEPTH_24) && defined(QT_NO_QWS_DEPTH_24)
# undef QT_QWS_DEPTH_24
#elif !defined(QT_QWS_DEPTH_24) && !defined(QT_NO_QWS_DEPTH_24)
# define QT_QWS_DEPTH_24
#endif

#if defined(QT_QWS_DEPTH_32) && defined(QT_NO_QWS_DEPTH_32)
# undef QT_QWS_DEPTH_32
#elif !defined(QT_QWS_DEPTH_32) && !defined(QT_NO_QWS_DEPTH_32)
# define QT_QWS_DEPTH_32
#endif

#if defined(QT_QWS_DEPTH_4) && defined(QT_NO_QWS_DEPTH_4)
# undef QT_QWS_DEPTH_4
#elif !defined(QT_QWS_DEPTH_4) && !defined(QT_NO_QWS_DEPTH_4)
# define QT_QWS_DEPTH_4
#endif

#if defined(QT_QWS_DEPTH_8) && defined(QT_NO_QWS_DEPTH_8)
# undef QT_QWS_DEPTH_8
#elif !defined(QT_QWS_DEPTH_8) && !defined(QT_NO_QWS_DEPTH_8)
# define QT_QWS_DEPTH_8
#endif

#if defined(QT_QWS_DEPTH_GENERIC) && defined(QT_NO_QWS_DEPTH_GENERIC)
# undef QT_QWS_DEPTH_GENERIC
#elif !defined(QT_QWS_DEPTH_GENERIC) && !defined(QT_NO_QWS_DEPTH_GENERIC)
# define QT_QWS_DEPTH_GENERIC
#endif

#if defined(QT_RUNTIME_XCURSOR) && defined(QT_NO_RUNTIME_XCURSOR)
# undef QT_RUNTIME_XCURSOR
#elif !defined(QT_RUNTIME_XCURSOR) && !defined(QT_NO_RUNTIME_XCURSOR)
# define QT_RUNTIME_XCURSOR
#endif

#if defined(QT_RUNTIME_XFIXES) && defined(QT_NO_RUNTIME_XFIXES)
# undef QT_RUNTIME_XFIXES
#elif !defined(QT_RUNTIME_XFIXES) && !defined(QT_NO_RUNTIME_XFIXES)
# define QT_RUNTIME_XFIXES
#endif

#if defined(QT_RUNTIME_XINERAMA) && defined(QT_NO_RUNTIME_XINERAMA)
# undef QT_RUNTIME_XINERAMA
#elif !defined(QT_RUNTIME_XINERAMA) && !defined(QT_NO_RUNTIME_XINERAMA)
# define QT_RUNTIME_XINERAMA
#endif

#if defined(QT_RUNTIME_XINPUT) && defined(QT_NO_RUNTIME_XINPUT)
# undef QT_RUNTIME_XINPUT
#elif !defined(QT_RUNTIME_XINPUT) && !defined(QT_NO_RUNTIME_XINPUT)
# define QT_RUNTIME_XINPUT
#endif

#if defined(QT_RUNTIME_XRANDR) && defined(QT_NO_RUNTIME_XRANDR)
# undef QT_RUNTIME_XRANDR
#elif !defined(QT_RUNTIME_XRANDR) && !defined(QT_NO_RUNTIME_XRANDR)
# define QT_RUNTIME_XRANDR
#endif

#if defined(QT_USE_MATH_H_FLOATS) && defined(QT_NO_USE_MATH_H_FLOATS)
# undef QT_USE_MATH_H_FLOATS
#elif !defined(QT_USE_MATH_H_FLOATS) && !defined(QT_NO_USE_MATH_H_FLOATS)
# define QT_USE_MATH_H_FLOATS
#endif

#ifndef Q_WS_QWS
# define Q_WS_QWS
#endif

#define QT_VISIBILITY_AVAILABLE

