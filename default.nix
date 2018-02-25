{ stdenv }:

stdenv.mkDerivation {
  name = "uttyl";
  src = ./.;
  builder = builtins.toFile "builder.sh" ''
    source $stdenv/setup
    mkdir -p $out/bin
    gcc -lutil -o $out/bin/parpty $src/parpty.c
    gcc -o $out/bin/tmuxdo $src/tmuxdo.c
    cp $src/urxvtdo $out/bin/urxvtdo
    cp $src/tmuxtry $out/bin/tmuxtry
  '';
}
