{$i types.inc}
var picfile,palfile:string;
    CreatePal:boolean;

function GetExtension(n:string):ext;
var p,ep:byte;e:ext;
begin
 if length(n)<4 then ep:=0 else ep:=length(n)-4;
 e:='';p:=length(n);
 while p>ep do begin insert(n[p],e,1); if n[p]='.' then break; dec(p); end;
 if (e='') or (e[1]<>'.') then e:='';
 GetExtension:=e;
end;

function removeext(n:String):String;
var p:byte;
begin
 p:=pos('.',n);
 if p=0 then removeext:=n else removeext:=copy(n,1,p-1);
end;

procedure smartguess;
var defext,e:ext;

begin
 CreatePal:=false;
 Palfile:=Paramstr(2);
 picfile:=paramstr(1);
 if palfile='/p' then begin CreatePal:=True; PalFile:='' end;
 if paramstr(3)='/p' then CreatePal:=true;

 if CreatePal then defext:='.pal' else defext:='.plt';

 if picfile='' then exit;
 e:=GetExtension(picfile);
 if e='' then picfile:=picfile+'.bmp';
 if palfile='' then palfile:=RemoveExt(picfile)+defext
 else
 begin
   e:=GetExtension(palfile);
   if e='' then palfile:=palfile+defext else
   if e='.pal' then CreatePal:=true;
 end;
end;

Procedure Stop(msg:String);
begin
 Writeln(msg);
 halt;
end;

Procedure Help;
begin
 Writeln('BMP2PLT - extracts palette from BMP file to .plt/.pal file.');
 Writeln('syntax:'#13#10+
	 'BMP2PLT BMPFile [PaletteFile] [/p]'#13#10+
	 'Add /p to generate .PAL instead of .PLT');
 Halt;
end;

var
	f:file;
	Pal:TPal;
	bmh:TBitmapFileHeader;
	bih:TBitmapInfoHeader;
	BMPPal:array[0..255] of TRGBQuad;
	i:Integer;
	LPal:LFD_pal;
BEGIN
 if paramcount<1 then help;
 SmartGuess;
 Assign(f,PicFile); Reset(f,1);
 if ioresult<>0 then Stop('Can''t open file '+PicFile);
 BlockRead(f,bmh,sizeof(bmh));
 if bmh.bftype<>'BM' then Stop(Picfile+' is not a BMP file');
 BlockRead(f,bih,sizeof(bih));
 if (bih.biPlanes<>1) and (bih.biBitCount<>8) then Stop(Picfile+' is not a 256 color BMP file');
 BlockRead(f,BMPPal,sizeof(BMPPal));

 for i:=0 to 255 do
 begin
  Pal[i].red:=BMPPal[i].red;
  Pal[i].green:=BMPPal[i].green;
  Pal[i].blue:=BMPPal[i].blue;
 end;
 Close(f);

 Assign(f,PalFile); Rewrite(f,1);
 if ioresult<>0 then Stop('Can''t create file '+PalFile);
 if CreatePal then
 begin
  for i:=0 to 255 do
  begin
   Pal[i].red:=Pal[i].red shr 2;
   Pal[i].green:=Pal[i].green shr 2;
   Pal[i].blue:=Pal[i].blue shr 2;
  end;
  BlockWrite(f,Pal,sizeof(Pal));
 end
 else
 begin
  LPal.first:=0;
  LPal.last:=255;
  LPal.z1:=0;
  LPal.pal:=pal;
  BlockWrite(f,Lpal,sizeof(Lpal));
 end;
Close(f);
END.