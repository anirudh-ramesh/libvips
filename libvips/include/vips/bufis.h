/* Buffered inputput from a VipsStreami
 *
 * J.Cupitt, 18/11/19
 */

/*

    This file is part of VIPS.

    VIPS is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
    02110-1301  USA

 */

/*

    These files are distributed with VIPS - http://www.vips.ecs.soton.ac.uk

 */

#ifndef VIPS_BUFIS_H
#define VIPS_BUFIS_H

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

#define VIPS_TYPE_BUFIS (vips_bufis_get_type())
#define VIPS_BUFIS( obj ) \
	(G_TYPE_CHECK_INSTANCE_CAST( (obj), \
	VIPS_TYPE_BUFIS, VipsBufis ))
#define VIPS_BUFIS_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_CAST( (klass), \
	VIPS_TYPE_BUFIS, VipsBufisClass))
#define VIPS_IS_BUFIS( obj ) \
	(G_TYPE_CHECK_INSTANCE_TYPE( (obj), VIPS_TYPE_BUFIS ))
#define VIPS_IS_BUFIS_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_TYPE( (klass), VIPS_TYPE_BUFIS ))
#define VIPS_BUFIS_GET_CLASS( obj ) \
	(G_TYPE_INSTANCE_GET_CLASS( (obj), \
	VIPS_TYPE_BUFIS, VipsBufisClass ))

#define VIPS_BUFIS_BUFFER_SIZE (4096)

/* Layer over streami: read with an input buffer.
 * 
 * Libraries like libjpeg do their own input buffering and need raw IO, but
 * others, like radiance, need to parse the input into lines. A buffered read
 * class is very convenient.
 */
typedef struct _VipsBufis {
	VipsObject parent_object;

	/*< private >*/

	/* The VipsStreami we wrap.
	 */
	VipsStreami *streami;

	/* The +1 means there's always a \0 byte at the end.
	 *
	 * Unsigned char, since we don't want >127 to be -ve.
	 *
	 * chars_in_buffer is how many chars we have in input_buffer,
	 * read_point is the current read position in that buffer.
	 */
	unsigned char input_buffer[VIPS_BUFIS_BUFFER_SIZE + 1];
	int chars_in_buffer;
	int read_point;

	/* Build lines of text here.
	 */
	unsigned char line[VIPS_BUFIS_BUFFER_SIZE + 1];

} VipsBufis;

typedef struct _VipsBufisClass {
	VipsObjectClass parent_class;

} VipsBufisClass;

GType vips_bufis_get_type( void );

VipsBufis *vips_bufis_new_from_streami( VipsStreami *streami );

void vips_bufis_unbuffer( VipsBufis *streamib );

int vips_bufis_getc( VipsBufis *streamib );
#define VIPS_BUFIS_GETC( S ) ( \
	(S)->read_point < (S)->chars_in_buffer ? \
		(S)->input_buffer[(S)->read_point++] : \
		vips_bufis_getc( S ) \
)
void vips_bufis_ungetc( VipsBufis *streamib );
#define VIPS_BUFIS_UNGETC( S ) { \
	if( (S)->read_point > 0 ) \
		(S)->read_point -= 1; \
}

int vips_bufis_require( VipsBufis *streamib, int require );
#define VIPS_BUFIS_REQUIRE( S, R ) ( \
	(S)->read_point + (R) <= (S)->chars_in_buffer ? \
		0 :  \
		vips_bufis_require( (S), (R) ) \
)
#define VIPS_BUFIS_PEEK( S ) ((S)->input_buffer + (S)->read_point)
#define VIPS_BUFIS_FETCH( S ) ((S)->input_buffer[(S)->read_point++])

const char *vips_bufis_get_line( VipsBufis *streamib ); 
char *vips_bufis_get_line_copy( VipsBufis *streamib ); 
const char *vips_bufis_get_non_whitespace( VipsBufis *streamib );
int vips_bufis_skip_whitespace( VipsBufis *streamib );

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /*VIPS_BUFIS_H*/
