//
// ActionHandler.hh for pekwm
// Copyright (C) 2003-2006 Claes Nästén <me{@}pekdon{.}net>
//
// This program is licensed under the GNU GPL.
// See the LICENSE file for more information.
//
// $Id$
//

#include "../config.h"

#ifndef _ACTIONHANDLER_HH_
#define _ACTIONHANDLER_HH_

#include "pekwm.hh"
#include "Action.hh"

#include <string>
#include <list>

class Client;
class Frame;
class PWinObj;
class PDecor;
class PMenu;
class WindowManager;

class ActionHandler
{
public:
    ActionHandler(WindowManager *wm);
    ~ActionHandler(void);

    static inline ActionHandler *instance(void) { return _instance; }

    void handleAction(const ActionPerformed &ap);

    static bool checkAEThreshold(int x, int y, int x_t, int y_t, uint t);
    static ActionEvent *findMouseAction(uint button, uint mod,
                                        MouseEventType type,
                                        std::list<ActionEvent> *actions);
private:
    void handleStateAction(const Action &action, PWinObj *wo,
                           Client *client, Frame *frame);

    void actionFindClient(const std::string &title);
    void actionGotoClientID(uint id);
    void actionSendToWorkspace(PDecor *decor, int direction);
    void actionWarpToWorkspace(PDecor *decor, uint direction);
    void actionWarpToViewport(PDecor *decor, uint direction);
    void actionSendToViewport(PWinObj *wo, int col_off, int row_off);
    void actionFocusToggle(uint button, uint raise, int off,
                           bool show_iconified, bool mru);
    void actionFocusDirectional(PWinObj *wo, DirectionType dir, bool raise);
#ifdef MENUS
    void actionShowMenu(const std::string &name, bool stick,
                        uint e_type, PWinObj *wo_ref);
#endif // MENUS

    // action helpers
    Client *findClientFromTitle(const std::string &title);
    void gotoClient(Client *client);

    PMenu *createNextPrevMenu(bool show_iconified);
    PMenu *createMRUMenu(bool show_iconified);
    bool createMenuInclude(Frame *frame, bool show_iconified);

private:
    static ActionHandler *_instance;

    WindowManager *_wm;
};

#endif // _ACTIONHANDLER_HH_