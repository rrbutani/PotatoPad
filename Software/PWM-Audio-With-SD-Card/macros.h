// macros.h
// Runs on LM4F120/TM4C123
// Contains all the macros used by PotatoDoom
// Steven Zhu

#define range(x, min, max)	(x >= min && x <= max) ? x : (x < min ? min : max)
//	| x1 | x | x2 |
//  | y1 |	 | y2 |
#define crossProduct(x1, y1, x2, y2) (x1 * y2 - y1 * x2)
#define dotProduct(x1, y1, x2, y2) (x1 * x2 + y1 * y2)
//#define intersect (px1,py1,px2,py2,qx1,qy1,qx2,qy2) \
//{ \
//	(((crossProduct((px1-qx1),(py1-qx1),(qx2-qx1),(qy2-qy1)) > 0) != \
//	(crossProduct((px2-qx1),(py2-qx1),(qx2-qx1),(qy2-qy1)) > 0)) &&	\
//	((crossProduct((qx1-px1),(qy1-px1),(px2-px1),(py2-py1)) > 0) != (crossProduct((qx2-px1),(qy2-px1),(px2-px1),(py2-py1)) > 0))) \
//}
