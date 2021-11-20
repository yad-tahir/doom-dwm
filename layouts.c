/* See LICENSE file for copyright and license details. */

#include <stdio.h>
#include <X11/Xft/Xft.h>


void
tile(Monitor *m)
{
	unsigned int i, n, h, mw, my, ty;
	Client *c;

	for (n = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), n++);

	if (n == 0)
		return;

	if (n > m->nmaster)
		mw = m->nmaster ? m->ww * m->mfact : 0;
	else
		mw = m->ww;
	for (i = my = ty = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), i++)
		if (i < m->nmaster) {
			h = (m->wh - my) / (MIN(n, m->nmaster) - i);
			resize(c, m->wx, m->wy + my, mw - (2*c->bw), h - (2*c->bw), 0);
			if (my + HEIGHT(c) < m->wh)
				my += HEIGHT(c);
		} else {
			h = (m->wh - ty) / (n - i);
			resize(c, m->wx + mw, m->wy + ty, m->ww - mw - (2*c->bw), h - (2*c->bw), 0);
			if (ty + HEIGHT(c) < m->wh)
				ty += HEIGHT(c);
		}
}

void
monocle(Monitor *m)
{
	unsigned int n = 0;
	Client *c;

	for (c = m->clients; c; c = c->next)
		if (ISVISIBLE(c))
			n++;
	if (n > 0) /* override layout symbol */
		snprintf(m->ltsymbol, sizeof m->ltsymbol, "[%d]", n);
	for (c = nexttiled(m->clients); c; c = nexttiled(c->next))
		resize(c, m->wx, m->wy, m->ww - 2 * c->bw, m->wh - 2 * c->bw, 0);
}

Bool isPortrait(Monitor *m)
{
	return m->mw < m->mh;
}

void
fibonacci(Monitor *m, int s)
{
	unsigned int i, // current client index
	n, // number of tiled win
	h, w, x, y,
	mw, mh; // master area
	Client *c;

	// Calculate the number of tiled clients
	for (n = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), n++);

	if (n == 0)
		return;

	if (n > m->nmaster) {
		mw = m->nmaster ? m->ww * m->mfact : 0;
		mh = m->nmaster ? m->wh * m->mfact : 0;
	} else {
		// when there are no masters
		mw = m->ww;
		mh = m->wh;
	}


	int overlap = nexttiled(m->clients)->bw;
	for (i = x = y = h = w = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), i++) {
		if (i < m->nmaster) {
			// Check if the sceen is in portrait mode
			if (!isPortrait(m)) {
				w = (mw - x) / (MIN(n, m->nmaster)-i);
				resize(c, x + m->wx, m->wy,
					   w - (2*c->bw) + overlap, m->wh - (2*c->bw), False);
				x += WIDTH(c) - overlap;
			} else{
				h = (mh - y) / (MIN(n, m->nmaster)-i);
				resize(c, m->wx, y + m->wy,
					   m->ww - (2*c->bw), h - (2*c->bw) + overlap, False);
				y += HEIGHT(c) - overlap;
			}
		} else {
			// The var j is introduced so that the number of masters does not
			// affect slaves are drawn
			int j = i - m->nmaster;

			// Calculate the remaining dimension for the slaves
			if (j == 0) {
				w = m->ww - x;
				h = m->wh - y;
			}
			// If there is enough space
			if ((j % 2 && w / 2 > 2 * c->bw)
				|| (!(j % 2) && h / 2 > 2 * c->bw)) {
				// Avoid dividing so that the last client takes all the
				// remaining area
				if (i < n-1) {
					// update Size
					if (!isPortrait(m)) {
						if (j % 2) w /= 2;
						else h /= 2;

						if (j % 4 == 1 && !s) x +=w;
						if (j % 4 == 2 && !s) y +=h;
					} else {
						if (j % 2) h /= 2;
						else w /= 2;

						if (j % 4 == 1 && !s) y +=h;
						if (j % 4 == 2 && !s) x +=w;
					}
				}

				resize(c, m->wx + x, m->wy + y,
					   w - (2*c->bw) + overlap, h - (2*c->bw) + overlap, False);

				// Adjust the location for the next client
				if (!isPortrait(m)) {
					if (s) {
						if (j % 4 == 0) y += h;
						if (j % 4 == 1) x += w;
						if (j % 4 == 2) y += h;
						if (j % 4 == 3) x += w;
					} else {
						if (j % 4 == 0) y += h;
						if (j % 4 == 1) x -= w;
						if (j % 4 == 2) y -= h;
						if (j % 4 == 3) x += w;
					}
				} else {
					if (s) {
						if (j % 4 == 0) x += w;
						if (j % 4 == 1) y += h;
						if (j % 4 == 2) x += w;
						if (j % 4 == 3) y += h;
					} else {
						if (j % 4 == 0) x += w;
						if (j % 4 == 1) y -= h;
						if (j % 4 == 2) x -= w;
						if (j % 4 == 3) y += h;
					}
				}
			}
		}
	}
}

void
dwindle(Monitor *mon)
{
	fibonacci(mon, 1);
}

void
spiral(Monitor *mon)
{
	fibonacci(mon, 0);
}

void
grid (Monitor *m)
{
	unsigned int i, n, cx, cy, cw, ch, aw, ah, cols, rows;
	Client *c;

	for (n = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next))
		n++;

	if (n == 0)
		return;

	/* grid dimensions */
	for (rows = 0; rows <= n/2; rows++)
		if (rows*rows >= n)
			break;
	cols = (rows && (rows - 1) * rows >= n) ? rows - 1 : rows;

	/* window geoms (cell height/width) */
	ch = m->wh / (rows ? rows : 1);
	cw = m->ww / (cols ? cols : 1);

	int overlap = nexttiled(m->clients)->bw;
	for (i = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next)) {
		cx = m->wx + (i / rows) * cw;
		cy = m->wy + (i % rows) * ch;
		/* adjust height/width of last row/column's windows */
		ah = ((i + 1) % rows == 0) ? m->wh - ch * rows : 0;
		aw = (i >= rows * (cols - 1)) ? m->ww - cw * cols : 0;
		resize(c, cx, cy, cw - (2*c->bw) + overlap + aw, ch - (2*c->bw) + overlap + ah, False);
		i++;
	}
}
