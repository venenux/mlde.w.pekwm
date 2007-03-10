//
// WindowManager.hh for pekwm
// Copyright (C) 2003-2006 Claes Nästén <me{@}pekdon{.}net>
//
// windowmanager.hh for aewm++
// Copyright (C) 2000 Frank Hale <frankhale@yahoo.com>
// http://sapphire.sourceforge.net/
//
// This program is licensed under the GNU GPL.
// See the LICENSE file for more information.
//
// $Id$
//

#include "../config.h"

#ifndef _WINDOWMANAGER_HH_
#define _WINDOWMANAGER_HH_

#include "pekwm.hh"
#include "Action.hh"
#include "Atoms.hh"

#include <list>
#include <map>

#ifdef HAVE_XRANDR
extern "C" {
#include <X11/extensions/Xrandr.h>
}
#endif // HAVE_XRANDR

class ActionHandler;
class AutoProperties;
class Config;
class ColorHandler;
class FontHandler;
class TextureHandler;
class Theme;
class Workspaces;

class PScreen;
class ScreenResources;
class PWinObj;
class PDecor;
class Frame;
class Client;
class ClassHint;
class PMenu; // used together with Focus actions

class AutoProperty; // for findGroup
class CmdDialog;
class StatusWindow;

class KeyGrabber;
#ifdef HARBOUR
class Harbour;
#endif // HARBOUR

void sigHandler(int signal);
int handleXError(Display *dpy, XErrorEvent *e);

class WindowManager
{
public:
class EdgeWO : public PWinObj {
    public:
        EdgeWO(Display *dpy, Window root, EdgeType edge);
        virtual ~EdgeWO(void);

        virtual void mapWindow(void);

        virtual ActionEvent *handleButtonPress(XButtonEvent *ev);
        virtual ActionEvent *handleButtonRelease(XButtonEvent *ev);
        virtual ActionEvent *handleEnterEvent(XCrossingEvent *ev);

        inline EdgeType getEdge(void) const { return _edge; }

    private:
        EdgeType _edge;
    };

class RootWO : public PWinObj {
    public:
        RootWO(Display *dpy, Window root);
        virtual ~RootWO(void);

        virtual void resize(uint width, uint height) {
            _gm.width = width; _gm.height = height;
        }

        virtual ActionEvent *handleButtonPress(XButtonEvent *ev);
        virtual ActionEvent *handleButtonRelease(XButtonEvent *ev);
        virtual ActionEvent *handleMotionEvent(XMotionEvent *ev);
        virtual ActionEvent *handleEnterEvent(XCrossingEvent *ev);
        virtual ActionEvent *handleLeaveEvent(XCrossingEvent *ev);
    };

    WindowManager(const std::string &command_line, const std::string &config_file);
    ~WindowManager(void);

    //! @brief Sets reload status, will reload from main loop.
    void reload(void) { _reload = true; }
    void restart(std::string command = "");
    //! @brief Sets shutdown status, will shutdown from main loop.
    void shutdown(void) { _shutdown = true; }

    // get "base" classes
    inline PScreen *getScreen(void) const { return _screen; }
    inline Config *getConfig(void) const { return _config; }
    inline Theme *getTheme(void) const { return _theme; }
    inline ActionHandler *getActionHandler(void)
    const { return _action_handler; }
    inline AutoProperties* getAutoProperties(void) const {
        return _autoproperties;
    }
    inline Workspaces *getWorkspaces(void) const { return _workspaces; }
    inline KeyGrabber *getKeyGrabber(void) const { return _keygrabber; }
#ifdef HARBOUR
    inline Harbour *getHarbour(void) const { return _harbour; }
#endif //HARBOUR

    inline const std::string &getRestartCommand(void) const { return _restart_command; }
    inline bool isStartup(void) const { return _startup; }

    // list iterators
    inline std::list<PWinObj*>::iterator mru_begin(void) { return _mru_list.begin(); }
    inline std::list<PWinObj*>::reverse_iterator mru_rbegin(void) { return _mru_list.rbegin(); }
    inline std::list<PWinObj*>::iterator mru_end(void) { return _mru_list.end(); }
    inline std::list<PWinObj*>::reverse_iterator mru_rend(void) { return _mru_list.rend(); }

    // menus
#ifdef MENUS
    inline PMenu* getMenu(const std::string &name) {
        std::map<std::string, PMenu*>::iterator it = _menu_map.find(name);
        if (it != _menu_map.end())
            return it->second;
        return NULL;
    }
#endif // MENUS

    // adds
    inline void addToFrameList(Frame *frame) {
        if (frame) {
            _mru_list.remove(frame);
            _mru_list.push_front(frame);
        }
    }

    // removes
    void removeFromFrameList(Frame *frame);

    PWinObj *findPWinObj(Window win);

    // If a window unmaps and has transients lets unmap them too!
    // true = hide | false = unhide
    void findFamily(std::list<Client*> &client_list, Window win);
    void findTransientsToMapOrUnmap(Window win, bool hide);
    void familyRaiseLower(Client *client, bool raise);

    Frame* findGroup(AutoProperty *property);

    inline bool isAllowGrouping(void) const { return _allow_grouping; }
    inline void setStateGlobalGrouping(StateAction sa) {
        if (ActionUtil::needToggle(sa, _allow_grouping)) {
            _allow_grouping = !_allow_grouping;
        }
    }

    void attachMarked(Frame *frame);
    void attachInNextPrevFrame(Client* client, bool frame, bool next);

    Frame* getNextFrame(Frame* frame, bool mapped, uint mask = 0);
    Frame* getPrevFrame(Frame* frame, bool mapped, uint mask = 0);

    void findWOAndFocus(PWinObj *wo);

#ifdef MENUS
    void hideAllMenus(void);
#endif // MENUS

    inline CmdDialog *getCmdDialog(void) { return _cmd_dialog; }
    inline StatusWindow *getStatusWindow(void) { return _status_window; }

    // Methods for the various hints
    inline Atom getMwmHintsAtom(void) { return _atom_mwm_hints; }

    // Extended Window Manager hints function prototypes
    void setEwmhSupported(void);
    void setEwmhActiveWindow(Window w);

    // public event handlers used when doing grabbed actions
    void handleKeyEvent(XKeyEvent *ev);
    void handleButtonPressEvent(XButtonEvent *ev);
    void handleButtonReleaseEvent(XButtonEvent *ev);

private:
    void setupDisplay(void);
    void scanWindows(void);
    void execStartFile(void);

    void doReload(void);

    void cleanup(void);

    // screen edge related
    void screenEdgeCreate(void);
    void screenEdgeDestroy(void);
    void screenEdgeResize(void);
    void screenEdgeMapUnmap(void);

    // event handling
    void doEventLoop(void);

    void handleMapRequestEvent(XMapRequestEvent *ev);
    void handleUnmapEvent(XUnmapEvent *ev);
    void handleDestroyWindowEvent(XDestroyWindowEvent *ev);

    void handleConfigureRequestEvent(XConfigureRequestEvent *ev);
    void handleClientMessageEvent(XClientMessageEvent *ev);

    void handleColormapEvent(XColormapEvent *ev);
    void handlePropertyEvent(XPropertyEvent *ev);
    void handleExposeEvent(XExposeEvent *ev);

    void handleMotionEvent(XMotionEvent *ev);

    void handleEnterNotify(XCrossingEvent *ev);
    void handleLeaveNotify(XCrossingEvent *ev);
    void handleFocusInEvent(XFocusChangeEvent *ev);
    void handleFocusOutEvent(XFocusChangeEvent *ev);

#ifdef HAVE_XRANDR
    void handleXRandrEvent(XRRScreenChangeNotifyEvent *ev);
#endif // HAVE_XRANDR

#ifdef MENUS
    void createMenus(void);
    void updateMenus(void);
    void updateMenusStandalone(CfgParser::Entry *cfg_root);
    void deleteMenus(void);
#endif // MENUS

    // private methods for the hints
    void initHints(void);

private:
    PScreen *_screen;
    ScreenResources *_screen_resources;
    KeyGrabber *_keygrabber;
    Config *_config;
    ColorHandler *_color_handler;
    FontHandler *_font_handler;
    TextureHandler *_texture_handler;
    Theme *_theme;
    ActionHandler *_action_handler;
    AutoProperties *_autoproperties;
    Workspaces *_workspaces;
#ifdef HARBOUR
    Harbour *_harbour;
#endif // HARBOUR
    CmdDialog *_cmd_dialog;
    StatusWindow *_status_window;

#ifdef MENUS
    std::map<std::string, PMenu*> _menu_map;

    static const char *MENU_NAMES_RESERVED[];
    static const unsigned int MENU_NAMES_RESERVED_COUNT;
#endif // MENUS

    static const std::string _wm_name;
    std::string _command_line, _restart_command;
    bool _startup; //!< Indicates startup status.
    bool _shutdown; //!< Set to wheter we want to shutdown.
    bool _reload; //!< Set to wheter we want to reload.

    std::list<PWinObj*> _mru_list;

    bool _allow_grouping; //<! Flag turning grouping on/off.

    std::list<EdgeWO*> _screen_edge_list;
    PWinObj *_root_wo;

    // Atoms and hints follow under here
    PekwmAtoms *_pekwm_atoms;
    IcccmAtoms *_icccm_atoms;
    EwmhAtoms *_ewmh_atoms;

    // Atom for motif hints
    Atom _atom_mwm_hints;

    // Windows for the different hints
    Window _extended_hints_win;
};

#endif // _WINDOWMANAGER_HH_