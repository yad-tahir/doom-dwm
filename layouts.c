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
			my += HEIGHT(c);
		} else {
			h = (m->wh - ty) / (n - i);
			resize(c, m->wx + mw, m->wy + ty, m->ww - mw - (2*c->bw), h - (2*c->bw), 0);
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
	x = y = h = w = 0;

	for (i = x = y = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), i++) {
		if (i < m->nmaster) {
			// Check if the sceen is in portrait mode
			if (! isPortrait(m)) {
				w = (mw - x) / (MIN(n, m->nmaster)-i);
				resize(c, x + m->wx, m->wy,
					   w - (2*c->bw), m->wh - (2*c->bw), False);
				x += WIDTH(c);
			} else{
				h = (mh - y) / (MIN(n, m->nmaster)-i);
				resize(c, m->wx, y + m->wy,
					   m->ww - (2*c->bw), h - (2*c->bw), False);
				y += HEIGHT(c);
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

				resize(c, m->wx + x, m->wy + y, w - (2*c->bw), h - (2*c->bw), False);

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

	/* grid dimensions */
	for (rows = 0; rows <= n/2; rows++)
		if (rows*rows >= n)
			break;
	cols = (rows && (rows - 1) * rows >= n) ? rows - 1 : rows;

	/* window geoms (cell height/width) */
	ch = m->wh / (rows ? rows : 1);
	cw = m->ww / (cols ? cols : 1);
	for (i = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next)) {
		cx = m->wx + (i / rows) * cw;
		cy = m->wy + (i % rows) * ch;
		/* adjust height/width of last row/column's windows */
		ah = ((i + 1) % rows == 0) ? m->wh - ch * rows : 0;
		aw = (i >= rows * (cols - 1)) ? m->ww - cw * cols : 0;
		resize(c, cx, cy, cw - 2 * c->bw + aw, ch - 2 * c->bw + ah, False);
		i++;
	}
}

void
col(Monitor *m)
{
	unsigned int i, n, h, w, x, y,mw;
	Client *c;

	for (n = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), n++);
	if (n == 0)
		return;
	if (n > m->nmaster)
		mw = m->nmaster ? m->ww * m->mfact : 0;
	else
		mw = m->ww;
	for (i = x = y = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), i++) {
		if (i < m->nmaster) {
			w = (mw - x) / (MIN(n, m->nmaster)-i);
			resize(c, x + m->wx, m->wy,
				   w - (2*c->bw), m->wh - (2*c->bw), False);
			x += WIDTH(c);
		} else {
			h = (m->wh - y) / (n - i);
			resize(c, x + m->wx, m->wy + y,
				   m->ww - x - (2*c->bw), h - (2*c->bw), False);
			y += HEIGHT(c);
		}
	}
}

void
centeredmaster(Monitor *m)
{
	unsigned int i, n, h, mw, mx, my, oty, ety, tw;
	Client *c;

	/* count number of clients in the selected monitor */
	for (n = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), n++);
	if (n == 0)
		return;

	/* initialize areas */
	mw = m->ww; // master width
	mx = 0;
	my = 0;
	tw = mw; // slave target width?

	if (n > m->nmaster) {
		/* go mfact box in the center if more than nmaster clients */
		mw = m->nmaster ? m->ww * m->mfact : 0;
		tw = m->ww - mw;

		if (n - m->nmaster > 1) {
			/* only one client */
			mx = (m->ww - mw) / 2;
			tw = (m->ww - mw) / 2;
		}
	}

	oty = 0;
	ety = 0;
	for (i = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), i++)
	if (i < m->nmaster) {
		/* nmaster clients are stacked vertically, in the center
		 * of the screen */
		h = (m->wh - my) / (MIN(n, m->nmaster) - i);
		resize(c, m->wx + mx, m->wy + my, mw - (2*c->bw),
			   h - (2*c->bw), 0);
		my += HEIGHT(c);
	} else {
		/* stack clients are stacked vertically */
		if ((i - m->nmaster) % 2 ) {
			h = (m->wh - ety) / ( (1 + n - i) / 2);
			resize(c, m->wx, m->wy + ety, tw - (2*c->bw),
				   h - (2*c->bw), 0);
			ety += HEIGHT(c);
		} else {
			h = (m->wh - oty) / ((1 + n - i) / 2);
			resize(c, m->wx + mx + mw, m->wy + oty,
				   tw - (2*c->bw), h - (2*c->bw), 0);
			oty += HEIGHT(c);
		}
	}
}

void
centeredfloatingmaster(Monitor *m)
{
	unsigned int i, n, w, mh, mw, mx, mxo, my, myo, tx;
	Client *c;

	/* count number of clients in the selected monitor */
	for (n = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), n++);
	if (n == 0)
		return;

	/* initialize nmaster area */
	if (n > m->nmaster) {
		/* go mfact box in the center if more than nmaster clients */
		if (m->ww > m->wh) {
			mw = m->nmaster ? m->ww * m->mfact : 0;
			mh = m->nmaster ? m->wh * 0.9 : 0;
		} else {
			mh = m->nmaster ? m->wh * m->mfact : 0;
			mw = m->nmaster ? m->ww * 0.9 : 0;
		}
		mx = mxo = (m->ww - mw) / 2;
		my = myo = (m->wh - mh) / 2;
	} else {
		/* go fullscreen if all clients are in the master area */
		mh = m->wh;
		mw = m->ww;
		mx = mxo = 0;
		my = myo = 0;
	}

	for (i = tx = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), i++)
	if (i < m->nmaster) {
		/* nmaster clients are stacked horizontally, in the center
		 * of the screen */
		w = (mw + mxo - mx) / (MIN(n, m->nmaster) - i);
		resize(c, m->wx + mx, m->wy + my, w - (2*c->bw),
			   mh - (2*c->bw), 0);
		mx += WIDTH(c);
	} else {
		/* stack clients are stacked horizontally */
		w = (m->ww - tx) / (n - i);
		resize(c, m->wx + tx, m->wy, w - (2*c->bw),
			   m->wh - (2*c->bw), 0);
		tx += WIDTH(c);
	}
}
