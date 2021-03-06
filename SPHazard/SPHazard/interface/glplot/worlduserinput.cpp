#include "worldwindow.h"

void WorldWindow::OnMouseScroll(double scrollx ,double scrolly)
{

    MouseStateMutex.lock();

    if(m_MouseState.movescroll_first)
    {
        m_MouseState.ScrollOld_x = scrollx;
        m_MouseState.ScrollOld_y = scrolly;
        m_MouseState.movescroll_first = false;
    }

    m_MouseState.Scroll_x = scrollx;
    m_MouseState.Scroll_y = scrolly;

    m_MouseState.scroll += scrolly;

    m_MouseState.MoveScroll_X += scrollx;
    m_MouseState.MoveScroll_Y += scrolly;

    MouseStateMutex.unlock();

}
void WorldWindow::OnMouseMove(double posx,double posy)
{

    bool shift = glfwGetKey(m_OpenGLCLManager->window,GLFW_KEY_LEFT_SHIFT) ==GLFW_PRESS;

    MouseStateMutex.lock();
    if(m_MouseState.move_first == true)
    {
        m_MouseState.PosDO_x = posx;
        m_MouseState.PosDO_y = posy;
        m_MouseState.PosDOO_x = posx;
        m_MouseState.PosDOO_y = posy;
        m_MouseState.move_first = false;
    }


    m_MouseState.Pos_x = posx;
    m_MouseState.Pos_y = posy;

    double dx = m_MouseState.Pos_x - m_MouseState.PosDO_x;
    double dy = m_MouseState.Pos_y - m_MouseState.PosDO_y;

    m_MouseState.Move_X -=dx;
    m_MouseState.Move_Y +=dy;

    if(m_MouseState.dragging)
    {

        m_MouseState.Drag_x -=dx;
        m_MouseState.Drag_y +=dy;
        m_MouseState.PosDO_x = m_MouseState.Pos_x;
        m_MouseState.PosDO_y = m_MouseState.Pos_y;
    }else {

        m_MouseState.PosDOO_x = m_MouseState.Pos_x;
        m_MouseState.PosDOO_y = m_MouseState.Pos_y;
    }

    if(m_SunDrag)
    {

        m_SunDir = m_Camera3D->GetRayFromWindow(m_MouseState.Pos_x,m_MouseState.Pos_y).xyz();
        m_SunDir.y = std::max(0.0f,m_SunDir.y);
        m_SunDir = m_SunDir.Normalize();
    }

    float drag_x = m_MouseState.Pos_x - m_MouseState.PosDOO_x;
    float drag_y = m_MouseState.Pos_y - m_MouseState.PosDOO_y;
    float drag_s = std::sqrt(drag_x*drag_x + drag_y*drag_y);


    if(drag_s > 4 && m_DraggingFocusSquare == false && shift && m_LayerEditor == nullptr)
    {
        //add focus area
        m_FocusMutex.lock();
        m_FocusSquare.clear();
        m_DraggingFocusSquare = true;
        m_FocusSquareStart = SPHVector2(m_CurrentWindowState.MouseGeoPosX,m_CurrentWindowState.MouseGeoPosZ);
        m_FocusSquare.append(BoundingBox(m_FocusSquareStart.x,m_CurrentWindowState.MouseGeoPosX,m_FocusSquareStart.y,m_CurrentWindowState.MouseGeoPosZ));
        m_FocusMutex.unlock();

        emit OnFocusLocationChanged();

    }else if(m_DraggingFocusSquare && m_LayerEditor == nullptr)
    {
        if(!shift)
        {
            m_FocusMutex.lock();
            m_DraggingFocusSquare = false;

            m_FocusMutex.unlock();

            emit OnFocusLocationChanged();

        }else if(m_FocusSquare.length() > 0 && m_LayerEditor == nullptr)
        {
            BoundingBox b = m_FocusSquare.last();
            //b.MergeWith(SPHVector2(m_CurrentWindowState.MouseGeoPosX,m_CurrentWindowState.MouseGeoPosZ));
            b = BoundingBox(m_FocusSquareStart.x,m_CurrentWindowState.MouseGeoPosX,m_FocusSquareStart.y,m_CurrentWindowState.MouseGeoPosZ);
            m_FocusSquare.replace(m_FocusSquare.length()-1,b);
        }

    }
    MouseStateMutex.unlock();

}
void WorldWindow::OnMouseInOut(bool enter)
{

    MouseStateMutex.lock();

    m_MouseState.in = enter;

    MouseStateMutex.unlock();
}
void WorldWindow::OnMouseKey( int key, int action, int mods)
{

    MouseStateMutex.lock();
    bool shift = glfwGetKey(m_OpenGLCLManager->window,GLFW_KEY_LEFT_SHIFT) ==GLFW_PRESS;
    bool c = glfwGetKey(m_OpenGLCLManager->window,GLFW_KEY_C) ==GLFW_PRESS;
    if(shift)
    {
        m_MouseState.MouseButtonEventsShift.append(true);

    }else {
        m_MouseState.MouseButtonEventsShift.append(false);
    }

    m_MouseState.MouseButtonEvents.append(key);
    m_MouseState.MouseButtonKeyAction.append(action);

    if (key == GLFW_MOUSE_BUTTON_1  && action == GLFW_PRESS) {
        m_MouseState.dragging = true;
        m_MouseState.move_first = true;

        if(shift)
        {
            SPHVector4 ray = this->m_Camera3D->GetRayFromWindow(m_MouseState.Pos_x,m_MouseState.Pos_y);
            if(m_SunDir.Angle(ray.xyz()) < 0.05)
            {
                m_SunDrag = true;
            }
        }

    }else if(key == GLFW_MOUSE_BUTTON_1  && action == GLFW_RELEASE) {
        m_MouseState.dragging = false;
        m_SunDrag = false;
        if(shift && m_LayerEditor == nullptr)
        {
            float drag_x = m_MouseState.Pos_x - m_MouseState.PosDOO_x;
            float drag_y = m_MouseState.Pos_y - m_MouseState.PosDOO_y;
            float drag_s = std::sqrt(drag_x*drag_x + drag_y*drag_y);

            m_FocusMutex.lock();
            m_DraggingFocusSquare = false;
            m_FocusMutex.unlock();

            if(drag_s < 4)
            {

                //add focus point
                m_FocusMutex.lock();
                m_FocusLocations.append(SPHVector2(m_CurrentWindowState.MouseGeoPosX,m_CurrentWindowState.MouseGeoPosZ));
                m_FocusMutex.unlock();

                if(c)
                {
                    QClipboard *clipboard = QApplication::clipboard();
                    clipboard->setText("Point(" + QString::number(m_CurrentWindowState.MouseGeoPosX) + ","+QString::number(m_CurrentWindowState.MouseGeoPosZ)+ ")");
                }

            }else {


                if(c)
                {
                    QClipboard *clipboard = QApplication::clipboard();
                    if(m_FocusSquare.length() > 0)
                    {
                        BoundingBox b = m_FocusSquare.last();
                        clipboard->setText("BoundingBox(" + QString::number(b.GetMinX()) + ","+QString::number(b.GetMaxX())+","+QString::number(b.GetMinY()) + ","+QString::number(b.GetMaxY())+ ")");
                    }

                }
            }

            emit OnFocusLocationChanged();
        }else
        {

            m_FocusMutex.lock();
            m_DraggingFocusSquare = false;
            m_FocusMutex.unlock();
        }
    }
    if (key == GLFW_MOUSE_BUTTON_2  && action == GLFW_PRESS) {

    }else if (key == GLFW_MOUSE_BUTTON_2  && action == GLFW_RELEASE) {
        if(shift)
        {
            m_FocusMutex.lock();
            m_FocusSquare.clear();
            m_FocusLocations.clear();
            m_DraggingFocusSquare = false;
            m_FocusMutex.unlock();

            emit OnFocusLocationChanged();
        }
    }

    MouseStateMutex.unlock();
}
void WorldWindow::OnFrameBufferSize(int w,int h)
{

    MouseStateMutex.lock();

    MouseStateMutex.unlock();
}
void WorldWindow::OnKey(int key, int action, int mods)
{
    MouseStateMutex.lock();

    if(glfwGetKey(m_OpenGLCLManager->window,GLFW_KEY_LEFT_SHIFT) ==GLFW_PRESS)
    {

        m_MouseState.KeyEventShift.append(true);
    }else {

        m_MouseState.KeyEventShift.append(false);
    }


    m_MouseState.KeyEvents.append(key);
    m_MouseState.KeyAction.append(action);
    m_MouseState.KeyMods.append(mods);

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {



    }

    MouseStateMutex.unlock();
}

void WorldWindow::InputToLayers()
{

    //first check if we have to add or remove some focus point/area

    //if there is no editor


    //if there has been a mouse button release with left shift press

    //if there was a significant move, make it a square

    //otherwise make it a point

    //if there has been a right mouse button release with left shift press


    //input to layers

    bool shift = false;
    if(glfwGetKey(m_OpenGLCLManager->window,GLFW_KEY_LEFT_SHIFT) ==GLFW_PRESS)
    {
        shift = true;
    }

    //first get geo location
    bool GeoHit = m_CurrentWindowState.MouseHit;
    SPHVector3 GeoLoc = SPHVector3(m_CurrentWindowState.MouseGeoPosX,m_CurrentWindowState.MouseGeoPosY,m_CurrentWindowState.MouseGeoPosZ);

    for(int j = 0; j < m_UILayerList.length(); j++)
    {
        UILayer * l = m_UILayerList.at(j);
        l->OnFrame(m_GLDT,m_OpenGLCLManager->window);
        if(std::abs(m_MouseState.Move_X) > 0 || std::abs(m_MouseState.Move_Y) > 0)
        {
            l->OnMouseMove(SPHVector2(m_MouseState.Pos_x,m_MouseState.Pos_y),SPHVector2(m_MouseState.Move_X,m_MouseState.Move_Y));
            if(GeoHit)
            {
                l->OnGeoMouseMove(m_CurrentWindowState,GeoLoc,GeoLoc-m_Prev_GeoLoc,m_MouseState.dragging);
            }
        }
        if(std::abs(m_MouseState.MoveScroll_X) > 0 || std::abs(m_MouseState.MoveScroll_Y) > 0)
        {
            if(shift)
            {
                l->OnMouseScroll(SPHVector2(m_MouseState.Scroll_x,m_MouseState.Scroll_y),SPHVector2(m_MouseState.MoveScroll_X,m_MouseState.MoveScroll_Y));
            }
        }
        float dragx = m_MouseState.Drag_x;
        float dragy = m_MouseState.Drag_y;

        if(dragx != 0.0f || dragy != 0.0f)
        {

            if(shift)
            {
                l->OnMouseDrag(SPHVector2(m_MouseState.Pos_x,m_MouseState.Pos_y),SPHVector2(dragx,dragy));
            }
        }
        for(int i = 0; i < m_MouseState.MouseButtonEvents.length(); i++)
        {
            if((m_MouseState.MouseButtonEventsShift.at(i)))
            {
                l->OnMousePressed(m_MouseState.MouseButtonEvents.at(i),m_MouseState.MouseButtonKeyAction.at(i));
                if(GeoHit)
                {
                    l->OnGeoMousePressed(m_MouseState.MouseButtonEvents.at(i),m_MouseState.MouseButtonKeyAction.at(i),m_CurrentWindowState,GeoLoc);
                }
            }
        }
       for(int i = 0; i < m_MouseState.KeyEvents.length(); i++)
        {
            //if((m_MouseState.KeyEventShift.at(i)))
            {

                l->OnKeyPressed(m_MouseState.KeyEvents.at(i),m_MouseState.KeyAction.at(i),m_MouseState.KeyMods.at(i));
            }
        }
    }

    //input to editor and current tool

    UILayerEditor * l = m_LayerEditor;
    if(l != nullptr)
    {
        l->OnFrame(m_GLDT,m_OpenGLCLManager->window);
        if(std::abs(m_MouseState.Move_X) > 0 || std::abs(m_MouseState.Move_Y) > 0)
        {
            l->OnMouseMove(SPHVector2(m_MouseState.Pos_x,m_MouseState.Pos_y),SPHVector2(m_MouseState.Move_X,m_MouseState.Move_Y));
            if(GeoHit)
            {
                l->OnGeoMouseMove(m_CurrentWindowState,GeoLoc,GeoLoc-m_Prev_GeoLoc,m_MouseState.dragging);
            }
        }
        if(std::abs(m_MouseState.MoveScroll_X) > 0 || std::abs(m_MouseState.MoveScroll_Y) > 0)
        {
            if(shift)
            {
                l->OnMouseScroll(SPHVector2(m_MouseState.Scroll_x,m_MouseState.Scroll_y),SPHVector2(m_MouseState.MoveScroll_X,m_MouseState.MoveScroll_Y));
            }
        }
        float dragx = m_MouseState.Drag_x;
        float dragy = m_MouseState.Drag_y;

        if(dragx != 0.0f || dragy != 0.0f)
        {
            if(shift)
            {

                l->OnMouseDrag(SPHVector2(m_MouseState.Pos_x,m_MouseState.Pos_y),SPHVector2(dragx,dragy));
            }
        }
        for(int i = 0; i < m_MouseState.MouseButtonEvents.length(); i++)
        {
            if((m_MouseState.MouseButtonEventsShift.at(i)))
            {
                l->OnMousePressed(m_MouseState.MouseButtonEvents.at(i),m_MouseState.MouseButtonKeyAction.at(i));

                if(GeoHit)
                {
                    l->OnGeoMousePressed(m_MouseState.MouseButtonEvents.at(i),m_MouseState.MouseButtonKeyAction.at(i),m_CurrentWindowState,GeoLoc);
                }
            }
        }
        for(int i = 0; i < m_MouseState.KeyEvents.length(); i++)
        {
            if((m_MouseState.KeyEventShift.at(i)))
            {
                l->OnKeyPressed(m_MouseState.KeyEvents.at(i),m_MouseState.KeyAction.at(i),m_MouseState.KeyMods.at(i));
            }
        }
    }

    m_Prev_GeoLoc = GeoLoc;


}
