const minus:boolean=true;
Function openread(var f:file;s:string;dex:ext):integer;
begin
 openread:=0;
if s='' then
 begin
  write('Error: Empty filename ');
  openread:=-1; exit;
 end;
if (s[0]<>#1) and (not (s[1] in ['.','\'])) and (not (s[2]=':')) then s:=idir+s;
if pos('.',s)=0 then s:=s+dex;
assign(f,s);
reset(f,1);
if ioresult<>0 then
 begin
  write('Error opening file ',s); openread:=-1;
 end;
end;

Function openwrite(var f:file;s:string;dex:ext):integer;
begin
 openwrite:=0;
if s='' then
 begin
  write('Error: Empty filename ');
  openwrite:=-1; exit;
 end;
if (s[0]<>#1) and (not (s[1] in ['.','\'])) and (not (s[2]=':')) then s:=odir+s;
if pos('.',s)=0 then s:=s+dex;
assign(f,s);
rewrite(f,1);
if ioresult<>0 then
 begin
  write('Error creating file ',s); openwrite:=-1;
 end;
end;

function locase(c:char):char;
begin
 if c in ['A'..'Z'] then inc(c,32);
 locase:=c;
end;

procedure slocase(var s:string);
var i:integer;
begin
 for i:=1 to length(s) do s[i]:=locase(s[i]);
end;

procedure addslash(var s:fname);
begin
   if (s<>'') and not (s[length(s)] in [':','\']) then s:=concat(s,'\');
end;


procedure get8name(s:string; var n:name8);
var p:byte;i:byte; t:string[80];
begin
fillchar(n,sizeof(n),0);
fillchar(t,sizeof(t),0);
p:=pos('.',s);i:=0;
if p=0 then p:=length(s) else dec(p);
while (p>0) and (s[p]<>'\') and (s[p]<>':') do
 begin insert(locase(s[p]),t,1); dec(p); end;
n:=t;
end;

procedure getpath(s:fname;var p:fname);
var i:byte;
begin
i:=length(s);
while (i>0) and not (s[i] in ['\',':']) do dec(i);
p:=copy(s,1,i);
end;

function getext(n:fname):ext;
var p,ep:byte;e:ext;
begin
 if length(n)<4 then ep:=0 else ep:=length(n)-4;
 e:='';p:=length(n);
 while p>ep do begin insert(n[p],e,1); if n[p]='.' then break; dec(p); end;
 if (e='') or (e[1]<>'.') then e:='';
 getext:=e;
end;

function get13name(n:string):fname;
var s:name8;
begin
 get8name(n,s);
 get13name:=s+getext(n);
end;


function removeext(n:fname):fname;
var p:byte;
begin
 p:=pos('.',n);
 if p=0 then removeext:=n else removeext:=copy(n,1,p-1);
end;

procedure prepare(var s:string);
var i:integer;l:byte;
begin
 l:=pos('#',s);
 if l<>0 then s[0]:=chr(l-1);
 for i:=1 to length(s) do s[i]:=upcase(s[i]);
end;

function getword(var p:string; pos:byte; var s:string):byte;
var i:integer;
begin
 i:=pos; s:=''; getword:=0;
 while (p[i] in [' ',#9]) and (i<=length(p)) do inc(i);
 if i>length(p) then exit;
 s:=p[i]; inc(i);
 while not (p[i] in [' ',#9,'/']) and (i<=length(p)) do begin s:=concat(s,p[i]); inc(i) end;
 getword:=i;
end;

Procedure parse(var s:string;var pm:pmrecord);
var i,n:byte;w:string;
begin
 prepare(s);
 fillchar(pm,sizeof(pm),0);
 i:=1;n:=0;
 repeat
  i:=getword(s,i,w);
  if w='' then break;
  case w[1] of
   '/': begin pm.sw:=pm.sw+w[2]; pm.swpos[length(pm.sw)]:=i-length(w); end;
   '-': if minus then begin pm.sw:=pm.sw+w[2]; pm.swpos[length(pm.sw)]:=i-length(w); end
	else begin if n<=3 then pm.files[n]:=w; inc(n); end;
  else
    begin
     if n<=3 then pm.files[n]:=w;
     inc(n);
    end;
  end;
 until (w='') or (i>length(s));
end;

function nt2s(var n;i:byte):string;
var s:string;
begin
 fillchar(s,sizeof(s),0);
 move(n,s[1],i);
 s[0]:=chr(i);
 if pos(#0,s)<>0 then s[0]:=chr(pos(#0,s)-1);
 nt2s:=s;
end;

function tag2ext(tag:lfd_tag;var valid:boolean):ext;
type entry=record
		tag:lfd_tag;
		xt:ext;
	       end;
const table:array[1..8] of entry=(
		(tag:'ANIM';xt:'.anm'),
		(tag:'DELT';xt:'.dlt'),
		(tag:'FILM';xt:'.flm'),
		(tag:'FONT';xt:'.fon'),
		(tag:'PLTT';xt:'.plt'),
		(tag:'GMID';xt:'.gmd'),
		(tag:'VOIC';xt:'.voc'),
		(tag:'TEXT';xt:'.txt'));
var i:integer;
begin
tag2ext:='.'+nt2s(tag,3); valid:=false;
for i:=1 to sizeof(table) div sizeof(entry) do
begin
 if tag=table[i].tag then begin tag2ext:=table[i].xt; valid:=true; break; end;
end;
end;
