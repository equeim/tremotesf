Name: harbour-tremotesf
Summary: Transmission Remote for Sailfish OS
Version: 0.3.1
Release: 1
Group: Applications/Internet
License: GPLv3
URL: https://github.com/equeim/tremotesf
Source0: %{name}-%{version}.tar.xz
Requires: sailfishsilica-qt5
BuildRequires: pkgconfig(nemonotifications-qt5)
BuildRequires: pkgconfig(Qt5Quick)
BuildRequires: pkgconfig(sailfishapp)
BuildRequires: desktop-file-utils
BuildRequires: python

%description
Transmission Remote for Sailfish OS

%prep
%setup -q -n %{name}-%{version}

%build
./waf configure build --prefix=/usr --out=build-%{_arch}

%install
rm -rf %{buildroot}
./waf install --destdir=%{buildroot}
desktop-file-install --delete-original \
    --dir %{buildroot}%{_datadir}/applications \
    %{buildroot}%{_datadir}/applications/*.desktop

%files
%defattr(-,root,root,-)
%{_bindir}/%{name}
%{_datadir}/%{name}
%{_datadir}/applications/%{name}.desktop
%{_datadir}/icons/hicolor/*/apps/%{name}.png
