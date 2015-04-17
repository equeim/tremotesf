Name:       harbour-tremotesf
Summary:    Transmission Remote for Sailfish OS
Version:    0.1
Release:    1
Group:      Applications/Internet
License:    GPLv3
URL:        https://github.com/equeim/tremotesf
Source0:    %{name}-%{version}.tar.xz
Requires:   sailfishsilica-qt5
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5DBus)
BuildRequires:  pkgconfig(Qt5Network)
BuildRequires:  pkgconfig(Qt5Quick)
BuildRequires:  pkgconfig(sailfishapp)
BuildRequires:  pkgconfig(mlite5)
BuildRequires:  cmake
BuildRequires:  desktop-file-utils

%description
Transmission Remote for Sailfish OS

%prep
%setup -q -n %{name}-%{version}

%build
%cmake .. -DCMAKE_BUILD_TYPE=Release
make %{?_smp_mflags}

%install
rm -rf %{buildroot}

make install DESTDIR=%{buildroot}
desktop-file-install --delete-original \
  --dir %{buildroot}%{_datadir}/applications \
   %{buildroot}%{_datadir}/applications/*.desktop

%files
%defattr(-,root,root,-)
%{_bindir}/%{name}
%{_datadir}/%{name}
%{_datadir}/applications/%{name}.desktop
%{_datadir}/icons/hicolor/86x86/apps/%{name}.png
