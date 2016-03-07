#pragma once

#include "Direction.h"

/** \brief depresents a mouse
 * don't ever change the row/col of a mouse directly. This prevents it from working on the real robot
 * use forward and turnToFace to move the mouse around. Once those functions work on the real robot it will port over fluidly
 */
class Mouse {

  public:

    Mouse();

    /** \brief return the current column.
     * Guaranteed to be between 0 and MAZE_SIZE
     * \return current column
     */
    int getCol();

    /** \brief return the current row.
     * Guaranteed to be between 0 and MAZE_SIZE
     * \return current row
     */
    int getRow();

    /** \brief return the current direction.
     * \return current direction
     */
    Direction getDir();

    /** check if we're in bounds */
    bool inBounds();

    /** \brief is the mouse at the center of the maze? */
    bool atCenter();

    void turnToFace(char c);

    /** base functions the emulate mouse movement calls
     * returns -1 if you ran into a wall
     * returns -2 if the mouses's direction is messed up
    */
    virtual int forward() = 0;

    virtual void turnToFace(Direction d) = 0;

  protected:

    Mouse *instance;

    void internalTurnToFace(Direction dir);
    void internalForward();

    int row, col;
    Direction dir;

    static constexpr float ROT_TOLERANCE = 0.04;
};
