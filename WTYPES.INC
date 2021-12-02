const   tablesize=1024;

type
TBITMAPFILEHEADer=record
		 bftype:array[0..1] of char;{'BM'-Bitmap}
		 bfsize:longint;	{size in dwords}
		 bfreserved1,
		 bfreserved2:word;
		 bfoffbits:longint;	{offset of bits from header}
		END;

TBITMAPINFOHEADER=record
		 bisize,		{structure size}
		 biwidth,		{width in pels}
		 biheight:longint;	{height in pels}
		 biplanes,		{planes count}
		 bibitcount:word;	{bits per pel}
		 bicompression,		{compression type}
		 bisizeimage,		{picture size in bytes}
		 bixpelpermeter,	{pels per meter on x}
		 biypelpermeter,	{pels per meter on y}
		 biclrused,		{colors used in bitmap}
		 biclrimport:longint;	{important colors count}
		END;

TRGBQUAD=record
	 blue,
	 green,
	 red,
	 reservd:byte;
	END;

	pcxheader=record
	 manuf,
	 hard,
	 encod,
	 bitperpixel:byte;
	 x1,y1,x2,y2,
	 hres,vres:word;
	 palette:array[0..15] of record 	red,green,blue:byte; end;
	 vmode,
	 nplanes:byte;
	 byteperline,
	 palinfo,
	 shres,svres:word;
	 extra:array[0..53] of byte;
	end;


       FME_header1=record
	xshift,
	yshift,
	flip:longint;
	header2:longint;         (* this is the absolute adress of header2 *)
	fill:array[1..16] of char;
       end;
       FME_header2=record
	SizeX,
	SizeY,
	Compressed,
	DataSize:longint;
	fill:array[1..8] of char;
       end;
       Tpal=array[0..255] of record
				red,green,blue:byte;
			       end;
       lfd_pal=record
		 first:byte;
		 last:byte;
		 pal:tpal;
		 z1:byte;
	       end;


       TBITMAP=record
		w,h:word;
		pal:tpal;
		bits:pointer;
	       end;
       FME_details=record
		compressed:boolean;
		xshift,yshift,flip:longint;
		end;
       Wax_header=record
		   version:longint;
		   nseqs,
		   nframes,
		   ncells:longint;
		   xscale,yscale,xtralight,pad4:longint;
		   waxes:array [0..31] of longint;
		  end;
       twax=record
	     wwidth,wheight:longint;
	     framerate,   {? 8,0}
	     nframes:longint;
	     pad2,pad3,pad4:longint;
	     seqs:array[0..31] of longint;
	    end;
       tseq=record
	     pad1,pad2,pad3,pad4:longint;
	     frames:array[0..31] of longint;
	    end;

       Wax_data=record
		 ww,wh:longint;
		 Framerate:byte;
		 seqs:array[0..31] of byte;
		end;
       Seq_data=record
		 frames:array[0..31] of byte;
		end;
       Frame_data=record
		   xshift,yshift:integer;
		   flip:byte;
		   cell:byte;
		  end;
       Cell_data=record
		  w,h:word;
		  compressed:byte;
		 end;

       tbuf=array[0..60000] of byte;

       pmrecord=record
	sw:string[20];
	files:array[0..3] of string[80];
       end;

 ext=string[4];
 ident=string[10];
 ident_code=(C_WRONG,C_WAX,C_SET,C_IDIR,C_ODIR,C_PDIR,C_SEQS,C_WAXES,C_SEQ32,C_SEQ8,
	     C_SE8,C_FR,C_WW,C_WH,C_FRAMES,C_X,C_Y,C_FLIP,C_CELL,C_PAL,C_COMPRESSED,
	     C_BMP,C_SIMPLE,C_SWITCH,C_ANIMATED,C_FRAME,C_DELT,C_ANIM,C_LFD,C_MESSAGE,
	     C_XSHIFT,C_YSHIFT,C_CELLS,C_EMPTY,C_PCX,C_FILM);
 fstr=record
	handle:word;
	junk:array[0..45] of byte;
	name:array[0..79] of char;
      end;
 fname=string[80];
 name8=string[8];
	ttable=record
		n:word;
		i:array[0..tablesize-1] of longint;
	       end;
